# Tierra WebAssembly Port

このプロジェクトは、Tom RayのTierra人工生命シミュレーターをWebAssemblyに移植し、ブラウザ上で実行できるようにしたものです。

## 概要

Tierraは、自己複製プログラムが仮想コンピュータ環境内で進化するデジタル生命シミュレーターです。このWASM版では、GitHub Pages上でブラウザから直接アクセスして、進化の過程をリアルタイムで観察できます。

## 機能

- ✅ コアシミュレーションエンジン（約49,000行のCコード）
- ✅ リアルタイムビジュアライゼーション（Soup内の生命体を色分け表示）
- ✅ 統計情報表示（個体数、世代数、実行命令数など）
- ✅ シミュレーション制御（開始/停止/リセット/速度調整）
- ✅ レスポンシブデザイン（モバイル対応）

## プロジェクト構造

```
Tierra/
├── tierra/                  # 元のCソースコード（WASM対応に修正済み）
│   ├── wasm_frontend.c     # WASM専用フロントエンド
│   ├── tierra.c            # メインループ（WASM対応）
│   ├── configur.h          # ビルド設定（WASMタイプ追加）
│   └── ...
├── src/                     # Webフロントエンド
│   ├── index.html          # メインHTML
│   ├── js/
│   │   └── app.js          # アプリケーションロジック
│   └── styles/
│       └── main.css        # スタイルシート
├── build/                   # ビルド設定
│   ├── Makefile.wasm       # Emscriptenビルドスクリプト
│   └── soup_config.txt     # WASM用Tierra設定
├── dist/                    # ビルド出力（GitHub Pages公開用）
└── .github/
    └── workflows/
        └── deploy-wasm.yml # CI/CD自動ビルド
```

## ビルド方法

### 方法1: GitHub Actions（推奨）

1. `tierra-on-browser`ブランチにプッシュ
2. GitHub Actionsが自動的にWASMをビルド
3. GitHub Pagesにデプロイ

### 方法2: ローカルビルド

#### 前提条件

- Emscripten SDK 3.1.50以降

#### 手順

```bash
# Emscriptenのインストール（初回のみ）
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.50
./emsdk activate 3.1.50
source ./emsdk_env.sh

# Tierraプロジェクトに戻る
cd /path/to/Tierra

# ビルド実行
cd build
make -f Makefile.wasm

# ローカルサーバーで確認
cd ../dist
python3 -m http.server 8000
# ブラウザでhttp://localhost:8000 を開く
```

## GitHub Pages設定

1. リポジトリの Settings > Pages を開く
2. Source: "GitHub Actions" を選択
3. `tierra-on-browser`ブランチにプッシュすると自動デプロイ
4. デプロイ完了後、`https://<username>.github.io/Tierra/` でアクセス可能

## 実装の詳細

### Cコードの変更点

#### 1. `tierra/wasm_frontend.c`（新規作成）

WASMからエクスポートする関数を定義：

- `tierra_init()` - シミュレーション初期化
- `tierra_step(int steps)` - N回のイテレーション実行
- `tierra_get_soup(int* size)` - Soupメモリへのポインタ取得
- `tierra_get_stats()` - 統計情報取得
- `tierra_reset()` - シミュレーションリセット

#### 2. `tierra/tierra.c`の修正

- `main()`関数を`#ifndef __EMSCRIPTEN__`でラップ
- `life()`関数をシングルイテレーションモードに対応
  - ネイティブ: `while`ループで無限実行
  - WASM: `if`文で1回だけ実行（JavaScriptから繰り返し呼び出し）

#### 3. `tierra/configur.h`の修正

- `WASM = 99`フロントエンドタイプを追加

#### 4. `tierra/frontend.c`の修正

- `#ifdef __EMSCRIPTEN__`で`wasm_frontend.c`をインクルード
- それ以外の場合は既存のフロントエンドを使用

### JavaScriptアーキテクチャ

#### `app.js` - メインアプリケーション

- WASMモジュールのロード
- シミュレーションループ（`requestAnimationFrame`）
- Canvas描画とUI更新

#### 主要な関数

```javascript
class TierraApp {
    async init()              // WASM初期化
    start()                   // シミュレーション開始
    pause()                   // 一時停止
    reset()                   // リセット
    run()                     // メインループ（毎フレーム実行）
    renderSoup()              // Soup可視化
    updateStats()             // 統計情報更新
    getColorForInstruction()  // 命令ごとの色マッピング
}
```

### ビルド設定

#### Emscriptenフラグ

- `-O3`: 最適化レベル3
- `-DPLOIDY=1`: シングルトラックゲノム
- `-D__EMSCRIPTEN__`: WASM環境フラグ
- `-DFRONTEND=99`: WASMフロントエンド使用
- `-s WASM=1`: WebAssembly出力
- `-s MODULARIZE=1`: ES6モジュール形式
- `-s ALLOW_MEMORY_GROWTH=1`: 動的メモリ拡張
- `-s INITIAL_MEMORY=67108864`: 初期メモリ64MB
- `--embed-file`: 設定ファイルと初期ゲノムを埋め込み

## 技術的課題と解決策

### 1. メインループのブロッキング

**問題**: 元の`life()`関数は無限`while`ループでブラウザをフリーズさせる

**解決策**:
- WASM版では`if`文で1イテレーションのみ実行
- JavaScriptの`requestAnimationFrame()`から毎フレーム呼び出し
- フレームごとの命令数を調整可能（速度スライダー）

### 2. ファイルI/O依存

**問題**: 元のコードは大量の`fopen()`, `fprintf()`を使用

**解決策**:
- `soup_in`設定ファイルは`--embed-file`でコンパイル時に埋め込み
- ディスク書き込みは無効化（`DiskBank=0`, `TierraLog=0`）
- 将来的にはJavaScriptコールバックで遺伝子バンク機能を実装可能

### 3. 大規模Soup可視化

**問題**: 60,000バイトを60fpsで描画するのは高コスト

**解決策**:
- シンプルな可視化（命令値を色にマッピング）
- 将来的な最適化案：
  - ビューポートカリング（表示領域のみ描画）
  - WebGL活用
  - ダウンサンプリング

## 動作要件

- モダンブラウザ（Chrome 90+, Firefox 89+, Safari 15+）
- WebAssemblyサポート必須
- JavaScript有効

## パフォーマンス

- **目標**: 60fps描画、1000+命令/秒
- **初期メモリ**: 64MB（SoupSize 60,000に対応）
- **WASM バイナリサイズ**: ~2-3MB（予想）

## 今後の拡張案

- [ ] サイズヒストグラム表示
- [ ] 遺伝子バンク機能（ダウンロード）
- [ ] ズーム/パン機能
- [ ] Cell境界の可視化
- [ ] パフォーマンス最適化（WebGL）
- [ ] 設定パラメータのUI調整
- [ ] スナップショット保存/読み込み（sessionStorage）

## ライセンス

元のTierraシミュレーターは Tom Ray & Virtual Life の著作権物です。
このWASM移植版は元のライセンスに従います。

## 参考資料

- [Tierra公式サイト](http://life.ou.edu/tierra/)
- [Emscripten公式ドキュメント](https://emscripten.org/)
- [実装プラン詳細](/Users/bioerrorlog/.claude/plans/dapper-snuggling-pike.md)

## 開発者向けメモ

### デバッグ

ブラウザの開発者コンソールで：

```javascript
// WASMモジュールにアクセス
Module._tierra_init()           // 初期化
Module._tierra_step(100)        // 100ステップ実行
Module._tierra_get_stats()      // 統計取得
```

### トラブルシューティング

**WASM読み込みエラー**:
- ネットワークタブで`tierra.wasm`が404になっていないか確認
- CORSエラーの場合、ローカルサーバー経由でアクセス

**シミュレーションが動かない**:
- コンソールでエラーメッセージ確認
- `tierra_init()`が成功したか確認（戻り値が0）

**表示がおかしい**:
- ブラウザのキャッシュをクリア
- Canvasサイズを確認（DPR対応）

## 貢献

バグ報告や機能提案は GitHub Issues へお願いします。

---

**作成日**: 2025-12-28
**バージョン**: Phase 1 (ビルドインフラ構築完了)
