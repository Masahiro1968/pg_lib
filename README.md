# PostgreSQL pg_lib ライブラリ

PostgreSQL 17.10 を前提とした C言語向けデータベースアクセスライブラリです。

## 概要
- Postgresアクセスの主要な関数およびユーティリティを集約しています。
- 接続管理、クエリ実行、型変換、文字列操作などのモジュールで構成されています。

## 前提条件・動作環境
- **PostgreSQL 17.10** 以降
- **CMake**: 3.15 以上
- **Cコンパイラ**: C11 サポート（GCC, Clang など）
- **PostgreSQL (libpq)**: 開発生析パッケージ（例: `libpq-dev` / `postgresql-devel`）

## CMake ビルドオプション

| オプション名 | デフォルト値 | 説明 |
| :--- | :---: | :--- |
| `PG_LIB_BUILD_SAMPLES` | `ON` | `sample/` 配下のサンプルプログラムをビルドする |
| `PG_LIB_BUILD_UTILITIES` | `ON` | `pg_util` ライブラリをビルドする |
| `PG_LIB_ENABLE_SANITIZER` | `ON` | Debugビルド時に Address/UB Sanitizer を有効化する |
| `PG_LIB_WERROR` | `ON` | 警告をエラーとして扱う (`-Werror`) |

## ビルド手順

### 基本ビルド（Debug）

Debug版でモジュールを作成すると、実行バイナリの名前の最後に'**D**'が付きます。

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### リリースビルド

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### サンプルやSanitizerを無効にしてビルドしたい場合

既定では、サニタイザーをバイナリに盛り込んでライブラリやサンプルを作成します。
なお、ライブラリにサニタイザーを設定した場合は、そのライブラリを使用する実行モジュール
にも、サニタイザーの設定が必要になります。ご注意ください。

```bash
cmake -B build -S . \
  -DPG_LIB_BUILD_SAMPLES=OFF \
  -DPG_LIB_ENABLE_SANITIZER=OFF
```

_最終更新日 2026-08-01_