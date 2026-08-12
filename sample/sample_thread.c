/**
 * @file    sample_thread.c
 * @brief   スレッドの簡単なサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - OpenMPとの比較用に、スレッドの簡単なサンプルを用意しました。
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 1MB (バイト単位で指定)
#define STACK_SIZE_1MB (1024 * 1024)

#define NUM_THREADS 2

/**
 * @brief 各スレッドが排他制御してアクセスするデータです。
 * @details 各スレッドがアクセスの際、排他制御してcounterを制御します。
 */
typedef struct
{
    //! 排他制御する対象
    int counter;

    //! 排他制御用ミューテックス
    pthread_mutex_t mutex;
} SharedContext;

/**
 * @brief   各スレッドに渡す構造体
 * @details サンプルのworker_thread()に渡す構造体です。
 */
typedef struct
{
    //! アプリケーションで生成したスレッド固有番号
    int thread_id;

    //! スレッドに引き渡すパラメータ
    SharedContext *ctx;
} ThreadArg;

/**
 * @brief サンプルのスレッド関数
 * @param[in] arg ThreadArg構造体
 */
void *worker_thread(void *arg)
{
    ThreadArg *t_arg = (ThreadArg *)arg;
    SharedContext *ctx = t_arg->ctx;

    // 現在のスタックサイズを確認して表示（デバッグ用）
    pthread_attr_t g_attr;
    size_t actual_stack_size;
    pthread_getattr_np(pthread_self(), &g_attr);
    pthread_attr_getstacksize(&g_attr, &actual_stack_size);
    pthread_attr_destroy(&g_attr);

    printf("[Thread %d] Started. Stack Size: %zu bytes (%zu MB)\n",
           t_arg->thread_id, actual_stack_size, actual_stack_size / (1024 * 1024));

    for (int i = 0; i < 3; i++)
    {
        pthread_mutex_lock(&ctx->mutex);
        ctx->counter++;
        printf("[Thread %d] Counter: %d\n", t_arg->thread_id, ctx->counter);
        pthread_mutex_unlock(&ctx->mutex);

        usleep(50000); // 50ms
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    SharedContext shared_ctx;

    // 1. ミューテックス初期化
    shared_ctx.counter = 0;
    if (pthread_mutex_init(&shared_ctx.mutex, NULL) != 0)
    {
        perror("pthread_mutex_init failed");
        return EXIT_FAILURE;
    }

    // 2. スレッド属性 (pthread_attr_t) の準備とスタックサイズ設定
    pthread_attr_t attr;
    pthread_attr_init(&attr); // 属性オブジェクトの初期化

    // スタックサイズを 1MB に設定
    if (pthread_attr_setstacksize(&attr, STACK_SIZE_1MB) != 0)
    {
        perror("pthread_attr_setstacksize failed");
        pthread_attr_destroy(&attr);
        return EXIT_FAILURE;
    }

    // 3. 属性（1MBスタック）を指定してスレッド生成
    for (int i = 0; i < NUM_THREADS; i++)
    {
        args[i].thread_id = i + 1;
        args[i].ctx = &shared_ctx;

        // 第2引数に &attr を渡す
        if (pthread_create(&threads[i], &attr, worker_thread, &args[i]) != 0)
        {
            perror("pthread_create failed");
            pthread_attr_destroy(&attr);
            return EXIT_FAILURE;
        }
    }

    // 属性オブジェクトは生成完了後に破棄してOK（スレッド作成完了後は不要）
    pthread_attr_destroy(&attr);

    // 4. スレッド合流
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&shared_ctx.mutex);
    printf("All threads finished. Final counter: %d\n", shared_ctx.counter);

    return EXIT_SUCCESS;
}