# Coding Rules

## Purpose
このドキュメントは、プロジェクト全体で統一されたコーディング規約を適用するためのルールセットである。  
Copilot によるコードレビュー時は、このルールを基準としてチェックを行う。

---

## Rule 1: 変数名は必ずスネークケースでなければならない
### Description
ローカル変数・グローバル変数・メンバ変数など、すべての変数名は  
**snake_case（xxx_yyy）形式でなければならない。**

### Scope
など、プロジェクト内のすべてのコードファイル。

### Violations
以下のような形式はすべて違反とする：

### How to Check
Copilot は以下を確認する：

### Examples

#### OK
```cpp
int user_id;
QString file_path;
double max_value;
```


## Rule 2: C言語の関数名は必ずスネークケースでなければならない
### Description
C言語の関数名は、公開関数・静的関数を問わず、すべて
**小文字の snake_case（xxx_yyy）形式でなければならない。**

### Scope
- 関数の宣言
- 関数の定義
- `static` 関数
- コールバック関数など、関数ポインタへ設定する関数

### Violations
以下のような関数名はすべて違反とする：
- `camelCase`
- `PascalCase`
- `mixedCase`
- `xxxYyy`
- `xxx-yyy`
- `xxxYyyZzz`

### How to Check
Copilot はC言語の関数名について以下を確認する：
- 関数名が `^[a-z][a-z0-9_]*$` に一致するか
- 大文字が含まれていないか
- `_` を含まない単語が連結されていないか
- 記号（-、.、$ など）が含まれていないか
- 関数の宣言と定義で同じスネークケース名が使用されているか

関数形式マクロや外部ライブラリが提供する関数名は、プロジェクト内で命名変更できないため対象外とする。

### Examples

#### OK
```c
int pg_connect_database(const char *connection_info);
static void reset_connection_state(void);
```

#### NG
```c
int pgConnectDatabase(const char *connection_info);
static void resetConnectionState(void);
```

---

## Rule 3: C言語の関数にはDoxygenコメントを記載しなければならない
### Description
C言語の関数には、関数の目的とインターフェースが分かるDoxygenコメントを記載する。
特に公開関数には、呼び出し側が必要とする情報を不足なく記載しなければならない。

Doxygenコメントはヘッダーファイル側の関数宣言にのみ記載する。
Cソースファイル側の関数定義には、同じDoxygenコメントを重複して記載してはならない。
ヘッダーに宣言を置かないファイル内限定の関数は、Doxygenコメントの対象外とする。

### Scope
- ヘッダーファイルに宣言を置く公開関数
- ヘッダーファイルに宣言を置くコールバック関数
- 構造体、列挙型、列挙値、公開変数などの公開API

### Required Tags
関数の種類に応じて、以下のタグを使用する：
- `@brief`: 関数の目的を簡潔に記載する
- `@param[in]`: 入力引数の意味と制約を記載する
- `@param[out]`: 出力引数の意味と設定内容を記載する
- `@param[in,out]`: 入出力引数の意味と変更内容を記載する
- `@return`: 戻り値、成功時・失敗時の値や意味を記載する
- `@details`: `@brief` だけでは説明できない動作、前提条件、副作用を記載する

引数がない場合は `@param` を省略してよい。戻り値がない関数では `@return` を省略してよい。
ただし、戻り値や引数がある関数では、対応する説明を省略してはならない。

### How to Check
Copilot は以下を確認する：
- ヘッダーファイル側の関数宣言の直前に `/** ... */` 形式のDoxygenコメントがあるか
- Cソースファイル側の関数定義に、同じDoxygenコメントが重複して記載されていないか
- ヘッダーに宣言を置かないファイル内限定の関数を、Doxygenコメントの未記載として扱っていないか
- 関数の目的が実装内容と一致しているか
- すべての引数が適切な `@param` タグで説明されているか
- `@param[in]`、`@param[out]`、`@param[in,out]` の方向が実装と一致しているか
- 戻り値の意味、エラー値、NULLの扱いなどが `@return` に記載されているか
- メモリ確保・解放、入力値の変更、データベースアクセスなどの副作用が説明されているか
- コメントに記載された条件、値、エラー動作が実装と矛盾していないか
- 実装の細部を繰り返すだけでなく、呼び出し側に必要な契約を説明しているか

コメントの有無やタグの対応は機械的に確認し、説明内容と実装の一致や不足はAIによるレビューで確認する。
AIは推測で補完せず、実装から確認できない仕様をコメントに追加しない。

### Examples

#### OK
```c
/**
 * @brief  SQLを実行する。
 * @param[in] ctx データベース接続コンテキスト。NULL不可。
 * @param[in] sql 実行するSQL文字列。NULL不可。
 * @return 成功時はtrue、失敗時はfalse。
 */
bool pg_exec(PGContext *ctx, const char *sql);
```

#### NG
```c
/* SQLを実行する */
bool pg_exec(PGContext *ctx, const char *sql);
```
