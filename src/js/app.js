// app.js - Main application entry point

class TierraApp {
    constructor() {
        this.module = null;
        this.running = false;
        this.animationId = null;
        this.stepsPerFrame = 10;
        this.canvas = document.getElementById('soup-canvas');
        this.ctx = this.canvas.getContext('2d');
        this.lastFrameTime = 0;
        this.fps = 0;
    }

    async init() {
        console.log('Initializing Tierra WASM...');
        document.getElementById('loading').textContent = 'Loading WASM...';

        try {
            // Load the WASM module
            this.module = await TierraModule({
                onRuntimeInitialized: () => {
                    console.log('Tierra WASM initialized');
                    this.onWasmReady();
                },
                print: (text) => console.log('[Tierra]', text),
                printErr: (text) => console.error('[Tierra Error]', text)
            });
        } catch (error) {
            console.error('Failed to load WASM:', error);
            document.getElementById('loading').textContent = 'Error loading WASM: ' + error.message;
        }
    }

    onWasmReady() {
        console.log('Calling tierra_init()...');

        try {
            const result = this.module._tierra_init();
            console.log('tierra_init() returned:', result);

            document.getElementById('loading').style.display = 'none';
            this.setupControls();
            this.initCanvas();
            console.log('Tierra ready!');
        } catch (error) {
            console.error('Error initializing Tierra:', error);
            document.getElementById('loading').textContent = 'Error: ' + error.message;
        }
    }

    setupControls() {
        document.getElementById('btn-start').addEventListener('click', () => {
            this.start();
        });

        document.getElementById('btn-pause').addEventListener('click', () => {
            this.pause();
        });

        document.getElementById('btn-reset').addEventListener('click', () => {
            if (confirm('Reset simulation? This will lose current state.')) {
                this.reset();
            }
        });

        document.getElementById('speed-slider').addEventListener('input', (e) => {
            const speed = parseInt(e.target.value) / 10;
            this.setSpeed(speed);
            document.getElementById('speed-display').textContent = speed.toFixed(1) + 'x';
        });
    }

    initCanvas() {
        // Set up canvas for high DPI displays
        const dpr = window.devicePixelRatio || 1;
        const rect = this.canvas.getBoundingClientRect();

        this.canvas.width = rect.width * dpr;
        this.canvas.height = rect.height * dpr;
        this.ctx.scale(dpr, dpr);

        this.canvas.style.width = rect.width + 'px';
        this.canvas.style.height = rect.height + 'px';

        // Initial render
        this.renderSoup();
    }

    start() {
        if (this.running) return;
        this.running = true;
        this.toggleButtons(true);
        this.lastFrameTime = performance.now();
        this.run();
    }

    pause() {
        this.running = false;
        this.toggleButtons(false);
        if (this.animationId) {
            cancelAnimationFrame(this.animationId);
            this.animationId = null;
        }
    }

    reset() {
        this.pause();
        try {
            this.module._tierra_reset();
            this.renderSoup();
            this.updateStats();
        } catch (error) {
            console.error('Error resetting simulation:', error);
        }
    }

    setSpeed(multiplier) {
        this.stepsPerFrame = Math.max(1, Math.floor(10 * multiplier));
    }

    run() {
        if (!this.running) return;

        try {
            // Execute simulation steps
            this.module._tierra_step(this.stepsPerFrame);

            // Update visualization
            this.renderSoup();
            this.updateStats();

            // Calculate FPS
            const now = performance.now();
            const delta = now - this.lastFrameTime;
            this.fps = 1000 / delta;
            this.lastFrameTime = now;
            document.getElementById('stat-fps').textContent = this.fps.toFixed(1);

        } catch (error) {
            console.error('Error in simulation loop:', error);
            this.pause();
        }

        // Schedule next frame
        this.animationId = requestAnimationFrame(() => this.run());
    }

    updateStats() {
        try {
            const statsPtr = this.module._tierra_get_stats();
            if (!statsPtr) return;

            // Read stats structure (5 I32s: num_cells, generations, inst_exe_m, inst_exe_i, soup_size)
            const stats = {
                numCells: this.module.HEAP32[statsPtr >> 2],
                generations: this.module.HEAP32[(statsPtr >> 2) + 1],
                instExeM: this.module.HEAP32[(statsPtr >> 2) + 2],
                instExeI: this.module.HEAP32[(statsPtr >> 2) + 3],
                soupSize: this.module.HEAP32[(statsPtr >> 2) + 4]
            };

            // Update DOM
            document.getElementById('stat-population').textContent = stats.numCells;
            document.getElementById('stat-generations').textContent = stats.generations;

            const instTotal = stats.instExeM * 1000000 + stats.instExeI;
            document.getElementById('stat-instructions').textContent =
                (instTotal / 1000000).toFixed(2) + 'M';

            document.getElementById('stat-soupsize').textContent = stats.soupSize;

        } catch (error) {
            console.error('Error updating stats:', error);
        }
    }

    renderSoup() {
        try {
            const sizePtr = this.module._malloc(4);
            const soupPtr = this.module._tierra_get_soup(sizePtr);
            const soupSize = this.module.HEAP32[sizePtr >> 2];

            if (!soupPtr || soupSize === 0) {
                this.module._free(sizePtr);
                return;
            }

            // Read soup memory
            const soup = new Uint8Array(this.module.HEAPU8.buffer, soupPtr, soupSize);

            // Render soup to canvas
            const width = this.canvas.width / (window.devicePixelRatio || 1);
            const height = this.canvas.height / (window.devicePixelRatio || 1);

            this.ctx.fillStyle = '#000';
            this.ctx.fillRect(0, 0, width, height);

            // Simple visualization: draw soup as colored pixels
            const pixelsPerByte = Math.max(1, width / soupSize);
            const bytesToShow = Math.min(soupSize, Math.floor(width / pixelsPerByte));

            for (let i = 0; i < bytesToShow; i++) {
                const byte = soup[i];
                const x = i * pixelsPerByte;

                // Color mapping: different colors for different instruction values
                const color = this.getColorForInstruction(byte);

                this.ctx.fillStyle = color;
                this.ctx.fillRect(x, 0, Math.ceil(pixelsPerByte), height);
            }

            this.module._free(sizePtr);

        } catch (error) {
            console.error('Error rendering soup:', error);
        }
    }

    getColorForInstruction(inst) {
        if (inst === 0) return '#000000'; // Empty

        // Nop instructions (0-31): grey scale
        if (inst < 32) {
            const grey = inst * 8;
            return `rgb(${grey}, ${grey}, ${grey})`;
        }

        // Executable instructions: color by opcode group
        const hue = (inst % 16) * 22.5; // 16 different hues
        const sat = 70;
        const light = 50;
        return `hsl(${hue}, ${sat}%, ${light}%)`;
    }

    toggleButtons(running) {
        document.getElementById('btn-start').disabled = running;
        document.getElementById('btn-pause').disabled = !running;
    }
}

// Start the application when DOM is ready
window.addEventListener('DOMContentLoaded', () => {
    const app = new TierraApp();
    app.init();
});
