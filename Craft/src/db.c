#include <string.h>
#include "db.h"
#include "ring.h"
#include "sqlite3.h"
#include "tinycthread.h"

static int db_enabled = 0;

static sqlite3 *db;
static sqlite3_stmt *insert_block_stmt;
static sqlite3_stmt *insert_light_stmt;
static sqlite3_stmt *insert_sign_stmt;
static sqlite3_stmt *delete_sign_stmt;
static sqlite3_stmt *delete_signs_stmt;
static sqlite3_stmt *load_blocks_stmt;
static sqlite3_stmt *load_lights_stmt;
static sqlite3_stmt *load_signs_stmt;
static sqlite3_stmt *get_key_stmt;
static sqlite3_stmt *set_key_stmt;
static sqlite3_stmt *get_metadata_stmt;
static sqlite3_stmt *set_metadata_stmt;
static sqlite3_stmt *insert_bible_pos_stmt;
static sqlite3_stmt *get_bible_pos_stmt;

static Ring ring;
static thrd_t thrd;
static mtx_t mtx;
static cnd_t cnd;
static mtx_t load_mtx;

void db_enable() {
    db_enabled = 1;
}

void db_disable() {
    db_enabled = 0;
}

int get_db_enabled() {
    return db_enabled;
}

int db_init(char *path) {
    if (!db_enabled) {
        return 0;
    }
    static const char *create_query =
        "attach database 'auth.db' as auth;"
        "create table if not exists auth.identity_token ("
        "   username text not null,"
        "   token text not null,"
        "   selected int not null"
        ");"
        "create unique index if not exists auth.identity_token_username_idx"
        "   on identity_token (username);"
        "create table if not exists state ("
        "   x float not null,"
        "   y float not null,"
        "   z float not null,"
        "   rx float not null,"
        "   ry float not null"
        ");"
        "create table if not exists block ("
        "    p int not null,"
        "    q int not null,"
        "    x int not null,"
        "    y int not null,"
        "    z int not null,"
        "    w int not null"
        ");"
        "create table if not exists light ("
        "    p int not null,"
        "    q int not null,"
        "    x int not null,"
        "    y int not null,"
        "    z int not null,"
        "    w int not null"
        ");"
        "create table if not exists key ("
        "    p int not null,"
        "    q int not null,"
        "    key int not null"
        ");"
        "create table if not exists sign ("
        "    p int not null,"
        "    q int not null,"
        "    x int not null,"
        "    y int not null,"
        "    z int not null,"
        "    face int not null,"
        "    text text not null"
        ");"
        "create table if not exists metadata ("
        "    key text not null primary key,"
        "    value text not null"
        ");"
        "create table if not exists bible_position ("
        "    book text not null,"
        "    chapter int not null,"
        "    verse int not null,"
        "    x int not null,"
        "    y int not null,"
        "    z int not null"
        ");"
        "create table if not exists daily_reading_blocks ("
        "    x int not null,"
        "    y int not null,"
        "    z int not null,"
        "    date text not null"
        ");"
        "create unique index if not exists block_pqxyz_idx on block (p, q, x, y, z);"
        "create unique index if not exists light_pqxyz_idx on light (p, q, x, y, z);"
        "create unique index if not exists key_pq_idx on key (p, q);"
        "create unique index if not exists sign_xyzface_idx on sign (x, y, z, face);"
        "create index if not exists sign_pq_idx on sign (p, q);"
        "create unique index if not exists bible_position_idx on bible_position (book, chapter, verse);";
    static const char *insert_block_query =
        "insert or replace into block (p, q, x, y, z, w) "
        "values (?, ?, ?, ?, ?, ?);";
    static const char *insert_light_query =
        "insert or replace into light (p, q, x, y, z, w) "
        "values (?, ?, ?, ?, ?, ?);";
    static const char *insert_sign_query =
        "insert or replace into sign (p, q, x, y, z, face, text) "
        "values (?, ?, ?, ?, ?, ?, ?);";
    static const char *delete_sign_query =
        "delete from sign where x = ? and y = ? and z = ? and face = ?;";
    static const char *delete_signs_query =
        "delete from sign where x = ? and y = ? and z = ?;";
    static const char *load_blocks_query =
        "select x, y, z, w from block where p = ? and q = ?;";
    static const char *load_lights_query =
        "select x, y, z, w from light where p = ? and q = ?;";
    static const char *load_signs_query =
        "select x, y, z, face, text from sign where p = ? and q = ?;";
    static const char *get_key_query =
        "select key from key where p = ? and q = ?;";
    static const char *set_key_query =
        "insert or replace into key (p, q, key) "
        "values (?, ?, ?);";
    static const char *get_metadata_query =
        "select value from metadata where key = ?;";
    static const char *set_metadata_query =
        "insert or replace into metadata (key, value) "
        "values (?, ?);";
    static const char *insert_bible_pos_query =
        "insert or replace into bible_position (book, chapter, verse, x, y, z) "
        "values (?, ?, ?, ?, ?, ?);";
    static const char *get_bible_pos_query =
        "select x, y, z from bible_position where book = ? and chapter = ? and verse = ?;";
    int rc;
    rc = sqlite3_open(path, &db);
    if (rc) return rc;
    rc = sqlite3_exec(db, create_query, NULL, NULL, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(
        db, insert_block_query, -1, &insert_block_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(
        db, insert_light_query, -1, &insert_light_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(
        db, insert_sign_query, -1, &insert_sign_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(
        db, delete_sign_query, -1, &delete_sign_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(
        db, delete_signs_query, -1, &delete_signs_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, load_blocks_query, -1, &load_blocks_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, load_lights_query, -1, &load_lights_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, load_signs_query, -1, &load_signs_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, get_key_query, -1, &get_key_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, set_key_query, -1, &set_key_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, get_metadata_query, -1, &get_metadata_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, set_metadata_query, -1, &set_metadata_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, insert_bible_pos_query, -1, &insert_bible_pos_stmt, NULL);
    if (rc) return rc;
    rc = sqlite3_prepare_v2(db, get_bible_pos_query, -1, &get_bible_pos_stmt, NULL);
    if (rc) return rc;
    sqlite3_exec(db, "begin;", NULL, NULL, NULL);
    db_worker_start();
    return 0;
}

void db_close() {
    if (!db_enabled) {
        return;
    }
    db_worker_stop();
    sqlite3_exec(db, "commit;", NULL, NULL, NULL);
    sqlite3_finalize(insert_block_stmt);
    sqlite3_finalize(insert_light_stmt);
    sqlite3_finalize(insert_sign_stmt);
    sqlite3_finalize(delete_sign_stmt);
    sqlite3_finalize(delete_signs_stmt);
    sqlite3_finalize(load_blocks_stmt);
    sqlite3_finalize(load_lights_stmt);
    sqlite3_finalize(load_signs_stmt);
    sqlite3_finalize(get_key_stmt);
    sqlite3_finalize(set_key_stmt);
    sqlite3_close(db);
}

void db_commit() {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&mtx);
    ring_put_commit(&ring);
    cnd_signal(&cnd);
    mtx_unlock(&mtx);
}

void _db_commit() {
    sqlite3_exec(db, "commit; begin;", NULL, NULL, NULL);
}

// Force immediate synchronous commit (for critical saves like generation progress)
void db_commit_sync() {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&load_mtx);
    _db_commit();
    // Force a full sync to disk
    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", NULL, NULL, NULL);
    mtx_unlock(&load_mtx);
}

void db_auth_set(char *username, char *identity_token) {
    if (!db_enabled) {
        return;
    }
    static const char *query =
        "insert or replace into auth.identity_token "
        "(username, token, selected) values (?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, NULL);
    sqlite3_bind_text(stmt, 2, identity_token, -1, NULL);
    sqlite3_bind_int(stmt, 3, 1);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    db_auth_select(username);
}

int db_auth_select(char *username) {
    if (!db_enabled) {
        return 0;
    }
    db_auth_select_none();
    static const char *query =
        "update auth.identity_token set selected = 1 where username = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return sqlite3_changes(db);
}

void db_auth_select_none() {
    if (!db_enabled) {
        return;
    }
    sqlite3_exec(db, "update auth.identity_token set selected = 0;",
        NULL, NULL, NULL);
}

int db_auth_get(
    char *username,
    char *identity_token, int identity_token_length)
{
    if (!db_enabled) {
        return 0;
    }
    static const char *query =
        "select token from auth.identity_token "
        "where username = ?;";
    int result = 0;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *a = (const char *)sqlite3_column_text(stmt, 0);
        strncpy(identity_token, a, identity_token_length - 1);
        identity_token[identity_token_length - 1] = '\0';
        result = 1;
    }
    sqlite3_finalize(stmt);
    return result;
}

int db_auth_get_selected(
    char *username, int username_length,
    char *identity_token, int identity_token_length)
{
    if (!db_enabled) {
        return 0;
    }
    static const char *query =
        "select username, token from auth.identity_token "
        "where selected = 1;";
    int result = 0;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *a = (const char *)sqlite3_column_text(stmt, 0);
        const char *b = (const char *)sqlite3_column_text(stmt, 1);
        strncpy(username, a, username_length - 1);
        username[username_length - 1] = '\0';
        strncpy(identity_token, b, identity_token_length - 1);
        identity_token[identity_token_length - 1] = '\0';
        result = 1;
    }
    sqlite3_finalize(stmt);
    return result;
}

void db_save_state(float x, float y, float z, float rx, float ry) {
    if (!db_enabled) {
        return;
    }
    static const char *query =
        "insert into state (x, y, z, rx, ry) values (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_exec(db, "delete from state;", NULL, NULL, NULL);
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_double(stmt, 1, x);
    sqlite3_bind_double(stmt, 2, y);
    sqlite3_bind_double(stmt, 3, z);
    sqlite3_bind_double(stmt, 4, rx);
    sqlite3_bind_double(stmt, 5, ry);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

int db_load_state(float *x, float *y, float *z, float *rx, float *ry) {
    if (!db_enabled) {
        return 0;
    }
    static const char *query =
        "select x, y, z, rx, ry from state;";
    int result = 0;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *x = sqlite3_column_double(stmt, 0);
        *y = sqlite3_column_double(stmt, 1);
        *z = sqlite3_column_double(stmt, 2);
        *rx = sqlite3_column_double(stmt, 3);
        *ry = sqlite3_column_double(stmt, 4);
        result = 1;
    }
    sqlite3_finalize(stmt);
    return result;
}

void db_insert_block(int p, int q, int x, int y, int z, int w) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&mtx);
    ring_put_block(&ring, p, q, x, y, z, w);
    cnd_signal(&cnd);
    mtx_unlock(&mtx);
}

void _db_insert_block(int p, int q, int x, int y, int z, int w) {
    sqlite3_reset(insert_block_stmt);
    sqlite3_bind_int(insert_block_stmt, 1, p);
    sqlite3_bind_int(insert_block_stmt, 2, q);
    sqlite3_bind_int(insert_block_stmt, 3, x);
    sqlite3_bind_int(insert_block_stmt, 4, y);
    sqlite3_bind_int(insert_block_stmt, 5, z);
    sqlite3_bind_int(insert_block_stmt, 6, w);
    sqlite3_step(insert_block_stmt);
}

void db_insert_light(int p, int q, int x, int y, int z, int w) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&mtx);
    ring_put_light(&ring, p, q, x, y, z, w);
    cnd_signal(&cnd);
    mtx_unlock(&mtx);
}

void _db_insert_light(int p, int q, int x, int y, int z, int w) {
    sqlite3_reset(insert_light_stmt);
    sqlite3_bind_int(insert_light_stmt, 1, p);
    sqlite3_bind_int(insert_light_stmt, 2, q);
    sqlite3_bind_int(insert_light_stmt, 3, x);
    sqlite3_bind_int(insert_light_stmt, 4, y);
    sqlite3_bind_int(insert_light_stmt, 5, z);
    sqlite3_bind_int(insert_light_stmt, 6, w);
    sqlite3_step(insert_light_stmt);
}

void db_insert_sign(
    int p, int q, int x, int y, int z, int face, const char *text)
{
    if (!db_enabled) {
        return;
    }
    sqlite3_reset(insert_sign_stmt);
    sqlite3_bind_int(insert_sign_stmt, 1, p);
    sqlite3_bind_int(insert_sign_stmt, 2, q);
    sqlite3_bind_int(insert_sign_stmt, 3, x);
    sqlite3_bind_int(insert_sign_stmt, 4, y);
    sqlite3_bind_int(insert_sign_stmt, 5, z);
    sqlite3_bind_int(insert_sign_stmt, 6, face);
    sqlite3_bind_text(insert_sign_stmt, 7, text, -1, NULL);
    sqlite3_step(insert_sign_stmt);
}

void db_delete_sign(int x, int y, int z, int face) {
    if (!db_enabled) {
        return;
    }
    sqlite3_reset(delete_sign_stmt);
    sqlite3_bind_int(delete_sign_stmt, 1, x);
    sqlite3_bind_int(delete_sign_stmt, 2, y);
    sqlite3_bind_int(delete_sign_stmt, 3, z);
    sqlite3_bind_int(delete_sign_stmt, 4, face);
    sqlite3_step(delete_sign_stmt);
}

void db_delete_signs(int x, int y, int z) {
    if (!db_enabled) {
        return;
    }
    sqlite3_reset(delete_signs_stmt);
    sqlite3_bind_int(delete_signs_stmt, 1, x);
    sqlite3_bind_int(delete_signs_stmt, 2, y);
    sqlite3_bind_int(delete_signs_stmt, 3, z);
    sqlite3_step(delete_signs_stmt);
}

void db_delete_all_signs() {
    if (!db_enabled) {
        return;
    }
    sqlite3_exec(db, "delete from sign;", NULL, NULL, NULL);
}

void db_load_blocks(Map *map, int p, int q) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(load_blocks_stmt);
    sqlite3_bind_int(load_blocks_stmt, 1, p);
    sqlite3_bind_int(load_blocks_stmt, 2, q);
    while (sqlite3_step(load_blocks_stmt) == SQLITE_ROW) {
        int x = sqlite3_column_int(load_blocks_stmt, 0);
        int y = sqlite3_column_int(load_blocks_stmt, 1);
        int z = sqlite3_column_int(load_blocks_stmt, 2);
        int w = sqlite3_column_int(load_blocks_stmt, 3);
        map_set(map, x, y, z, w);
    }
    mtx_unlock(&load_mtx);
}

void db_load_lights(Map *map, int p, int q) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(load_lights_stmt);
    sqlite3_bind_int(load_lights_stmt, 1, p);
    sqlite3_bind_int(load_lights_stmt, 2, q);
    while (sqlite3_step(load_lights_stmt) == SQLITE_ROW) {
        int x = sqlite3_column_int(load_lights_stmt, 0);
        int y = sqlite3_column_int(load_lights_stmt, 1);
        int z = sqlite3_column_int(load_lights_stmt, 2);
        int w = sqlite3_column_int(load_lights_stmt, 3);
        map_set(map, x, y, z, w);
    }
    mtx_unlock(&load_mtx);
}

void db_load_signs(SignList *list, int p, int q) {
    if (!db_enabled) {
        return;
    }
    sqlite3_reset(load_signs_stmt);
    sqlite3_bind_int(load_signs_stmt, 1, p);
    sqlite3_bind_int(load_signs_stmt, 2, q);
    while (sqlite3_step(load_signs_stmt) == SQLITE_ROW) {
        int x = sqlite3_column_int(load_signs_stmt, 0);
        int y = sqlite3_column_int(load_signs_stmt, 1);
        int z = sqlite3_column_int(load_signs_stmt, 2);
        int face = sqlite3_column_int(load_signs_stmt, 3);
        const char *text = (const char *)sqlite3_column_text(
            load_signs_stmt, 4);
        sign_list_add(list, x, y, z, face, text);
    }
}

int db_get_key(int p, int q) {
    if (!db_enabled) {
        return 0;
    }
    sqlite3_reset(get_key_stmt);
    sqlite3_bind_int(get_key_stmt, 1, p);
    sqlite3_bind_int(get_key_stmt, 2, q);
    if (sqlite3_step(get_key_stmt) == SQLITE_ROW) {
        return sqlite3_column_int(get_key_stmt, 0);
    }
    return 0;
}

void db_set_key(int p, int q, int key) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&mtx);
    ring_put_key(&ring, p, q, key);
    cnd_signal(&cnd);
    mtx_unlock(&mtx);
}

void _db_set_key(int p, int q, int key) {
    sqlite3_reset(set_key_stmt);
    sqlite3_bind_int(set_key_stmt, 1, p);
    sqlite3_bind_int(set_key_stmt, 2, q);
    sqlite3_bind_int(set_key_stmt, 3, key);
    sqlite3_step(set_key_stmt);
}

int db_get_metadata(const char *key, char *value, int value_length) {
    if (!db_enabled) {
        return 0;
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(get_metadata_stmt);
    sqlite3_bind_text(get_metadata_stmt, 1, key, -1, SQLITE_STATIC);
    if (sqlite3_step(get_metadata_stmt) == SQLITE_ROW) {
        const char *result = (const char *)sqlite3_column_text(get_metadata_stmt, 0);
        if (result && value && value_length > 0) {
            strncpy(value, result, value_length - 1);
            value[value_length - 1] = '\0';
        }
        mtx_unlock(&load_mtx);
        return 1;
    }
    mtx_unlock(&load_mtx);
    return 0;
}

void db_set_metadata(const char *key, const char *value) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(set_metadata_stmt);
    sqlite3_bind_text(set_metadata_stmt, 1, key, -1, SQLITE_STATIC);
    sqlite3_bind_text(set_metadata_stmt, 2, value, -1, SQLITE_STATIC);
    sqlite3_step(set_metadata_stmt);
    mtx_unlock(&load_mtx);
}

void db_insert_bible_position(const char *book, int chapter, int verse, int x, int y, int z) {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(insert_bible_pos_stmt);
    sqlite3_bind_text(insert_bible_pos_stmt, 1, book, -1, SQLITE_STATIC);
    sqlite3_bind_int(insert_bible_pos_stmt, 2, chapter);
    sqlite3_bind_int(insert_bible_pos_stmt, 3, verse);
    sqlite3_bind_int(insert_bible_pos_stmt, 4, x);
    sqlite3_bind_int(insert_bible_pos_stmt, 5, y);
    sqlite3_bind_int(insert_bible_pos_stmt, 6, z);
    sqlite3_step(insert_bible_pos_stmt);
    mtx_unlock(&load_mtx);
}

int db_get_bible_position(const char *book, int chapter, int verse, int *x, int *y, int *z) {
    if (!db_enabled) {
        return 0;
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(get_bible_pos_stmt);
    sqlite3_bind_text(get_bible_pos_stmt, 1, book, -1, SQLITE_STATIC);
    sqlite3_bind_int(get_bible_pos_stmt, 2, chapter);
    sqlite3_bind_int(get_bible_pos_stmt, 3, verse);
    if (sqlite3_step(get_bible_pos_stmt) == SQLITE_ROW) {
        *x = sqlite3_column_int(get_bible_pos_stmt, 0);
        *y = sqlite3_column_int(get_bible_pos_stmt, 1);
        *z = sqlite3_column_int(get_bible_pos_stmt, 2);
        mtx_unlock(&load_mtx);
        return 1;
    }
    mtx_unlock(&load_mtx);
    return 0;
}

void db_insert_daily_reading_block(int x, int y, int z, const char *date) {
    if (!db_enabled) {
        return;
    }
    static sqlite3_stmt *stmt = NULL;
    if (!stmt) {
        const char *query = "insert into daily_reading_blocks (x, y, z, date) values (?, ?, ?, ?);";
        sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, x);
    sqlite3_bind_int(stmt, 2, y);
    sqlite3_bind_int(stmt, 3, z);
    sqlite3_bind_text(stmt, 4, date, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    mtx_unlock(&load_mtx);
}

void db_delete_daily_reading_blocks(const char *date) {
    if (!db_enabled) {
        return;
    }
    static sqlite3_stmt *stmt = NULL;
    if (!stmt) {
        const char *query = "delete from daily_reading_blocks where date = ?;";
        sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, date, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    mtx_unlock(&load_mtx);
}

// Get all daily reading blocks for clearing
// Returns number of blocks retrieved
int db_get_all_daily_reading_blocks(int **out_x, int **out_y, int **out_z) {
    if (!db_enabled) {
        return 0;
    }

    // First, count how many blocks we have
    static sqlite3_stmt *count_stmt = NULL;
    if (!count_stmt) {
        const char *query = "select count(*) from daily_reading_blocks;";
        sqlite3_prepare_v2(db, query, -1, &count_stmt, NULL);
    }

    mtx_lock(&load_mtx);
    sqlite3_reset(count_stmt);
    int count = 0;
    if (sqlite3_step(count_stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(count_stmt, 0);
    }
    mtx_unlock(&load_mtx);

    if (count == 0) {
        return 0;
    }

    // Allocate arrays
    *out_x = (int *)malloc(count * sizeof(int));
    *out_y = (int *)malloc(count * sizeof(int));
    *out_z = (int *)malloc(count * sizeof(int));

    // Query all blocks
    static sqlite3_stmt *select_stmt = NULL;
    if (!select_stmt) {
        const char *query = "select x, y, z from daily_reading_blocks;";
        sqlite3_prepare_v2(db, query, -1, &select_stmt, NULL);
    }

    mtx_lock(&load_mtx);
    sqlite3_reset(select_stmt);

    int i = 0;
    while (sqlite3_step(select_stmt) == SQLITE_ROW && i < count) {
        (*out_x)[i] = sqlite3_column_int(select_stmt, 0);
        (*out_y)[i] = sqlite3_column_int(select_stmt, 1);
        (*out_z)[i] = sqlite3_column_int(select_stmt, 2);
        i++;
    }
    mtx_unlock(&load_mtx);

    return i;
}

void db_delete_all_daily_reading_blocks() {
    if (!db_enabled) {
        return;
    }
    static sqlite3_stmt *stmt = NULL;
    if (!stmt) {
        const char *query = "delete from daily_reading_blocks;";
        sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    }
    mtx_lock(&load_mtx);
    sqlite3_reset(stmt);
    sqlite3_step(stmt);
    mtx_unlock(&load_mtx);
}

void db_worker_start(char *path) {
    if (!db_enabled) {
        return;
    }
    ring_alloc(&ring, 1024);
    mtx_init(&mtx, mtx_plain);
    mtx_init(&load_mtx, mtx_plain);
    cnd_init(&cnd);
    thrd_create(&thrd, db_worker_run, path);
}

void db_worker_stop() {
    if (!db_enabled) {
        return;
    }
    mtx_lock(&mtx);
    ring_put_exit(&ring);
    cnd_signal(&cnd);
    mtx_unlock(&mtx);
    thrd_join(thrd, NULL);
    cnd_destroy(&cnd);
    mtx_destroy(&load_mtx);
    mtx_destroy(&mtx);
    ring_free(&ring);
}

int db_worker_run(void *arg) {
    int running = 1;
    while (running) {
        RingEntry e;
        mtx_lock(&mtx);
        while (!ring_get(&ring, &e)) {
            cnd_wait(&cnd, &mtx);
        }
        mtx_unlock(&mtx);
        switch (e.type) {
            case BLOCK:
                _db_insert_block(e.p, e.q, e.x, e.y, e.z, e.w);
                break;
            case LIGHT:
                _db_insert_light(e.p, e.q, e.x, e.y, e.z, e.w);
                break;
            case KEY:
                _db_set_key(e.p, e.q, e.key);
                break;
            case COMMIT:
                _db_commit();
                break;
            case EXIT:
                running = 0;
                break;
        }
    }
    return 0;
}
