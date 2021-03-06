/**
 * The contents of this file are subject to the terms of the Common Development and
 * Distribution License (the License). You may not use this file except in compliance with the
 * License.
 *
 * You can obtain a copy of the License at legal/CDDLv1.0.txt. See the License for the
 * specific language governing permission and limitations under the License.
 *
 * When distributing Covered Software, include this CDDL Header Notice in each file and include
 * the License file at legal/CDDLv1.0.txt. If applicable, add the following below the CDDL
 * Header, with the fields enclosed by brackets [] replaced by your own identifying
 * information: "Portions copyright [year] [name of copyright owner]".
 *
 * Copyright 2014 - 2016 ForgeRock AS.
 */

#ifndef UTILITY_H
#define UTILITY_H

#include "pcre.h"
#include "net_client.h"

#define AM_POLICY_CHANGE_KEY    "AM_POLICY_CHANGE_KEY"
#define AM_CACHE_TIMEFORMAT     "%Y-%m-%d %H:%M:%S"
#define ARRAY_SIZE(array)       sizeof(array) / sizeof(array[0])
#define AM_BASE_TEN             10
#define AM_SPACE_CHAR           " "
#define AM_COMMA_CHAR           ","
#define AM_PIPE_CHAR            "|"
#define AM_BITMASK_CHECK(v,m)   (((v) & (m)) == (m))             

#define AM_NULL_CHECK(...) \
  do { \
    void *p[] = { __VA_ARGS__ }; \
    int i; \
    for (i = 0; i < sizeof(p)/sizeof(*p); i++) { \
      if (p[i] == NULL) { \
        return NULL; \
      } \
    } \
  } while(0)

#define AM_FREE(...) \
  do { \
    void *p[] = { __VA_ARGS__ }; \
    int i; \
    for (i = 0; i < sizeof(p)/sizeof(*p); i++) { \
      am_free(p[i]); \
    } \
  } while(0)

#ifndef htonll
#define htonll(x) ((htonl(1) == 1) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#endif
#ifndef ntohll
#define ntohll(x) ((ntohl(1) == 1) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

struct cache_object_ctx {
    size_t alloc_size;
    size_t data_size;
    size_t offset;
    int error;
    int external;
    void *data;
    int (*read)(struct cache_object_ctx *, void *, size_t);
    size_t(*write)(struct cache_object_ctx *, const void *, size_t);
};

enum {
    CONF_NUMBER = 0, CONF_STRING, CONF_NUMBER_LIST, CONF_STRING_LIST,
    CONF_STRING_MAP, CONF_DEBUG_LEVEL, CONF_ATTR_MODE, CONF_AUDIT_LEVEL
};

enum {
    ADMIN_IIS_MOD_ERROR = 0,
    ADMIN_IIS_MOD_NONE,
    ADMIN_IIS_MOD_GLOBAL,
    ADMIN_IIS_MOD_LOCAL
};

enum {
    AM_EXACT_MATCH = 0,
    AM_EXACT_PATTERN_MATCH,
    AM_NO_MATCH
};

enum {
    AM_SET_ATTRS_NONE = 0,
    AM_SET_ATTRS_AS_HEADER,
    AM_SET_ATTRS_AS_COOKIE
};

/* shared memory area handle */

typedef struct {
    uint64_t local_size;
    uint64_t *global_size;
#ifdef _WIN32
    HANDLE h[4]; /* 0: mutex, 1: file, 2: file mapping, 3: file mapping (for global_size) */
    int32_t error;
#else
    int32_t fd;
    void *lock;
    int32_t error;
#endif
    void *pool;
    void *base_ptr;  
    char init;
    char name[4][AM_PATH_SIZE];
} am_shm_t;

struct am_cookie {
    char *name;
    char *value;
    char *domain;
    char *max_age;
    char *path;
    char is_secure;
    char is_http_only;
    struct am_cookie *next;
};

struct am_action_decision {
    uint64_t ttl;
    struct am_namevalue *advices;
    struct am_action_decision *next;
    int method;
    int action;
};

struct am_policy_result {
    uint64_t created;
    int index;
    int scope;
    char *resource;
    struct am_namevalue *response_attributes;
    struct am_namevalue *response_decisions; /*profile attributes*/
    struct am_action_decision *action_decisions;
    struct am_policy_result *next;
};

struct notification_worker_data {
    unsigned long instance_id;
    char *post_data;
    size_t post_data_sz;
};

struct logout_worker_data {
    unsigned long instance_id;
    char *token;
    char *openam;
    am_net_options_t *options;
};

struct audit_worker_data {
    unsigned long instance_id;
    char *logdata;
    char *openam;
    am_net_options_t *options;
};

struct url_validator_worker_data {
    unsigned long instance_id;
    uint64_t last;
    int url_index;
    int running;
    char *config_path;
};

typedef struct {
    uint64_t start;
    uint64_t stop;
    uint64_t freq;
    int state;
} am_timer_t;

void delete_am_cookie_list(struct am_cookie **list);
void delete_am_policy_result_list(struct am_policy_result **list);

void am_timer(uint64_t *t);
void am_timer_start(am_timer_t *t);
void am_timer_stop(am_timer_t *t);
void am_timer_pause(am_timer_t *t);
void am_timer_resume(am_timer_t *t);
double am_timer_elapsed(am_timer_t *t);
void am_timer_report(unsigned long instance_id, am_timer_t *t, const char *op);

const char *get_valid_openam_url(am_request_t *r);

am_status_t ip_address_match(const char *ip, const char **list, unsigned int listsize, unsigned long instance_id);

am_status_t get_token_from_url(am_request_t *rq);
am_status_t get_cookie_value(am_request_t *rq, const char *separator, const char *cookie_name,
        const char *cookie_header_val, char **value);
int parse_url(const char *u, struct url *url);
char *url_encode(const char *str);
char *url_decode(const char *str);

size_t am_bin_path(char* buffer, size_t len);

int string_replace(char **original, const char *pattern, const char *replace, size_t *sz);

uint32_t next_pow_2(uint32_t v);
uint32_t prev_pow_2(uint32_t v);
uint64_t get_total_system_memory();
uint64_t get_disk_free_space(const char *vol, uint64_t *);
uint64_t get_log_buffer_size();
void am_shm_unlock(am_shm_t *);
int am_shm_lock(am_shm_t *);
int am_shm_lock_timeout(am_shm_t *am, int timeout_msec);
am_shm_t *am_shm_create(const char *, uint64_t, int use_new_initialiser, uint64_t *, int *);
void am_shm_shutdown(am_shm_t *);
int am_shm_delete(char *name);
void *am_shm_alloc(am_shm_t *am, uint64_t usize);
void am_shm_free(am_shm_t *am, void *ptr);
void *am_shm_realloc(am_shm_t *am, void *ptr, uint64_t size);
void am_shm_set_user_offset(am_shm_t *r, unsigned int s);
void *am_shm_get_user_pointer(am_shm_t *am);
void am_shm_info(am_shm_t *);
void am_shm_destroy(am_shm_t* am);

int am_create_agent_dir(const char *sep, const char *path, char **created_name,
        char **created_name_simple, uid_t* uid, gid_t* gid, void (*log)(const char *, ...));

int decrypt_password(const char *key, char **password);
int encrypt_password(const char *key, char **password);
void decrypt_agent_passwords(am_config_t *r);

char is_big_endian();
uint64_t page_size(uint64_t size);
am_return_t match(unsigned long instance_id, const char *subject, const char *pattern);
char *match_group(pcre *x, int capture_groups, const char *subject, size_t *len);
int gzip_deflate(const char *uncompressed, size_t *uncompressed_sz, char **compressed);
int gzip_inflate(const char *compressed, size_t *compressed_sz, char **uncompressed);
void trim(char *a, char w);

int am_vasprintf(char **buffer, const char *fmt, va_list arg);

void am_secure_zero_memory(void *v, size_t sz);

void read_directory(const char *path, struct am_namevalue **list);

#if defined(_WIN32) || defined(__sun)
char *strndup(const char *s, size_t n);
#if defined(__sun) || (_MSC_VER < 1900)
size_t strnlen(const char *string, size_t maxlen);
#endif
#endif

char *stristr(char *str1, char *str2);
am_status_t concat(char **str, size_t *str_sz, const char *s2, size_t s2sz);

int copy_file(const char *from, const char *to);

void xml_entity_escape(char *temp_str, size_t str_len);

am_bool_t contains_ctl(const char *string);
int char_count(const char *string, int c, int *last);

char file_exists(const char *fn);
char *load_file(const char *filepath, size_t *data_sz);
ssize_t write_file(const char *filepath, const void *data, size_t data_sz);

int get_line(char **line, size_t *size, FILE *file);

int am_session_decode(am_request_t *r);

char policy_compare_url(am_request_t *r, const char *pattern, const char *resource);
const char *am_policy_strerror(char status);

char* am_strsep(char** sp, const char* sep);
char* am_strldup(const char* src);
char *am_strrstr(const char *str, const char *search);

int compare_property(const char *line, const char *property);

int am_make_path(const char *path, uid_t* uid, gid_t* gid, void (*log)(const char *, ...));
int am_delete_file(const char *fn);
int am_delete_directory(const char *path);

int get_valid_url_index(unsigned long instance_id);

int get_ttl_value(struct am_namevalue *session, const char *name, int def, int value_in_minutes);

int create_am_namevalue_node(const char *n, size_t ns, const char *v, size_t vs, struct am_namevalue **node);
int create_am_policy_result_node(const char *resource, size_t resource_size, struct am_policy_result **node);
int create_am_action_decision_node(am_bool_t a, int m, uint64_t ttl,
        struct am_action_decision **node);

void *am_parse_session_xml(unsigned long instance_id, const char *xml, size_t xml_sz);
void *am_parse_session_saml(unsigned long instance_id, const char *xml, size_t xml_sz);
void *am_parse_policy_xml(unsigned long instance_id, const char *xml, size_t xml_sz, int scope);

int am_audit_init(int id);
int am_audit_shutdown();
int am_audit_processor_init();
void am_audit_processor_shutdown();
int am_audit_register_instance(am_config_t *conf);
int am_add_remote_audit_entry(unsigned long instance_id, const char *agent_token,
        const char *agent_token_server_id, const char *file_name,
        const char *user_token, const char *format, ...);

int am_url_validator_init();
void am_url_validator_shutdown();

int am_scope_to_num(const char *scope);
const char *am_scope_to_str(int scope);

int remove_cookie(am_request_t *rq, const char *cookie_name, char **cookie_hdr);

int am_set_policy_cache_epoch(uint64_t epoch_start);
int am_check_policy_cache_epoch(uint64_t policy_created);

int am_get_agent_config(unsigned long instance_id, const char *config_file, am_config_t **cnf);

void remove_agent_instance_byname(const char *name);

void am_agent_init_set_value(unsigned long instance_id, int val);
int am_agent_init_get_value(unsigned long instance_id);
void am_agent_instance_init_lock();
void am_agent_instance_init_unlock();

int am_get_agent_config_cache_or_local(unsigned long instance_id, const char *config_file, am_config_t **cnf);

am_config_t *am_parse_config_xml(unsigned long instance_id, const char *xml, size_t xml_sz, char log_enable);

int am_get_pdp_cache_entry(am_request_t *r, const char *key, char **url, char **file, char **content_type, int *method);
int am_add_pdp_cache_entry(am_request_t *r, const char *key, const char *url, const char *file, const char *content_type, int method);
int am_add_session_policy_cache_entry(am_request_t *request, const char *key,
        struct am_policy_result *policy, struct am_namevalue *session);
int am_get_session_policy_cache_entry(am_request_t *request, const char *key,
        struct am_policy_result **policy, struct am_namevalue **session, uint64_t *ts);

int am_get_cache_entry(unsigned long instance_id, int valid, const char *key);
int am_add_cache_entry(unsigned long instance_id, const char *key);

int am_remove_cache_entry(unsigned long instance_id, const char *key);

void* mem2cpy(void* dest, const void* source1, size_t size1, const void* source2, size_t size2);
void* mem3cpy(void* dest, const void* source1, size_t size1, const void* source2, size_t size2, const void* source3, size_t size3);

void update_agent_configuration_ttl(am_config_t *c);
void update_agent_configuration_audit(am_config_t *c);
void update_agent_configuration_normalise_map_urls(am_config_t *c);
void update_agent_configuration_reorder_map_values(am_config_t *c);

char *get_global_name(const char *name, int id);
am_bool_t validate_directory_access(const char *path, int mask);

typedef struct property_map property_map_t;
property_map_t * property_map_create();
void property_map_parse(property_map_t * map, char * source, am_bool_t override, void (* logf)(const char * format, ...), char * data, size_t data_sz);
char * property_map_get_value(property_map_t * map, const char * key);
void property_map_visit(property_map_t * map, am_bool_t(* callback)(char * key, char * value, void * data), void * data);
void property_map_delete(property_map_t * map);
am_bool_t property_map_remove_key(property_map_t * map, const char * key);
char ** property_map_get_value_addr(property_map_t * map, const char * key);
char * property_map_write_to_buffer(property_map_t * map, size_t * data_sz);

uint32_t am_hash_buffer(const void *buf, size_t len);
uint32_t am_hash(const void *buf);

void cache_object_ctx_init(struct cache_object_ctx *ctx);
void cache_object_ctx_init_data(struct cache_object_ctx *ctx, void *data, size_t sz);
void cache_object_ctx_destroy(struct cache_object_ctx *ctx);
int cache_object_write_key(struct cache_object_ctx *ctx, char *key);
int cache_object_skip_key(struct cache_object_ctx *ctx);
int am_policy_result_serialise(struct cache_object_ctx *ctx, struct am_policy_result *list);
int am_name_value_serialise(struct cache_object_ctx *ctx, struct am_namevalue *list);
struct am_policy_result *am_policy_result_deserialise(struct cache_object_ctx *ctx);
struct am_namevalue *am_name_value_deserialise(struct cache_object_ctx *ctx);

int am_pdp_entry_serialise(struct cache_object_ctx *ctx, const char *url,
        const char *file, const char *content_type, int method);
int am_pdp_entry_deserialise(struct cache_object_ctx *ctx, char **url,
        char **file, char **content_type, int *method);

int am_policy_epoch_deserialise(struct cache_object_ctx *ctx, uint64_t *p_time);
int am_policy_epoch_serialise(struct cache_object_ctx *ctx, uint64_t time);

int am_cache_worker_init();
void am_cache_worker_shutdown();
int am_cache_cleanup(int id);

#endif
