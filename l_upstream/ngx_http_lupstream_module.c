/*
** Copy Right  CoderLiu(github)
**
*/
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define LUS_CONNECTION_TIMEOUT 600;
static char *ngx_http_lupstream(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static void lupstream_finalize_request(ngx_http_request_t *r, ngx_int_t rc);
static ngx_int_t lupstream_create_request(ngx_http_request_t *r);
static ngx_int_t lupstream_process_header(ngx_http_request_t *r);

typedef struct {
	ngx_http_upstream_conf_t us;
} ngx_http_lupstream_conf_t;

static ngx_command_t ngx_http_lupstream_commands[] = {
	{ngx_string("lupstream"),
	 NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
     ngx_http_lupstream,
	 0,
	 0,
     NULL},
	ngx_null_command
};

static ngx_http_module_t ngx_http_lupstream_module_ctx = {
	
	NULL,
	NULL,
	
	NULL,
	NULL,
	
	NULL,
	NULL,
	
    ngx_http_lupstream_create_loc_conf,
    ngx_http_lupstream_merge_loc_conf
};


ngx_module_t ngx_http_lupstream_module = {
	NGX_MODULE_V1,
	&ngx_http_lupstream_module_ctx,
	ngx_http_lupstream_commands,
	NGX_HTTP_MODULE,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NGX_MODULE_V1_PADDING 
};

static ngx_int_t ngx_http_lupstream_handler(ngx_http_request_t * r){

    r->upstream->create_request = lupstream_create_request;
    r->upstream->finalize_request = lupstream_finalize_request;
    r->upstream->process_header = lupstream_process_header;
    r->main->count++;//ref counter
    ngx_http_upstream_init(r);
	return NGX_OK;
}

static void * ngx_http_lupstream_create_loc_conf(ngx_conf_t *cf){
	ngx_http_lupstream_conf_t * lucf;

	lucf = (ngx_http_lupstream_conf_t *)ngx_pcalloc(cf->pool, sizeof(ngx_http_lupstream_conf_t));

	if (lucf == NULL)
	{
		return NULL;
	}
	lucf->us.connect_timeout = LUS_CONNECTION_TIMEOUT;
	lucf->us.send_timeout = LUS_CONNECTION_TIMEOUT;
	lucf->us.read_timeout = LUS_CONNECTION_TIMEOUT;
	lucf->us.store_access = 0600;
	lucf->us.buffering = 0;
	lucf->us.bufs.num = 8;
	lucf->us.bufs.size = ngx_pagesize;
	lucf->us.buffer_size = ngx_pagesize;
	lucf->us.busy_buffer_size = 2 * ngx_pagesize;
	lucf->us.temp_file_write_size = 2 * ngx_pagesize;
	lucf->us.max_temp_file_size = 1024 * 1024 * 1024;

	lucf->us.hide_headers = NGX_CONF_UNSET_PTR;
	lucf->us.pass_headers = NGX_CONF_UNSET_PTR;
	return lucf;
}

static char * ngx_http_lupstream_merge_loc_conf(ngx_conf_t *cf, void * parent, void * child){
	ngx_http_lupstream_conf_t *prev = (ngx_http_lupstream_conf_t *)parent;
	ngx_http_lupstream_conf_t *conf = (ngx_http_lupstream_conf_t *)child;

	ngx_hash_init_t hash;
	hash.max-size= 100;
	hash.bucket_size = 1024;
	hahs.name = "proxy_headers_hash";
	if (ngx_http_upstream_hide_headers_hash(cf, &conf->us, &prev->us, ngx_http_proxy_hide_headers, &hash) != NGX_OK)
	{
		return NGX_CONF_ERROR;
	}
	return NGX_CONF_OK;
}


static char* ngx_http_lupstream(ngx_conf_t *cf, ngx_command_t*cmd, void *conf){
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_lupstream_handler;
	return NGX_CONF_OK;
}


void lupstream_finalize_request(ngx_http_request_t *r, ngx_int_t rc){

}
ngx_int_t lupstream_create_request(ngx_http_request_t *r){
	static ngx_str_t str q = ngx_string('GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection:close\r\n\r\n');
	ngx_buf_t *b = ngx_create_temp_buf(r->pool, q.len);
	if (b == NULL){
		return NGX_ERROR;
	}
	b->last = p->pos + q.len;
	ngx_snprintf(b->pos, q.len, (char *)q.data);

	r->upstream->request_bufs = ngx_alloc_chain_link(r->pool);
	if (r->upstream->request_bufs == NULL){
		return NGX_ERROR;
	}

	r->upstream->reques_bufs->buf = b;
	r->upstream->reques_bufs->next = NULL;

	r->upstream->request_sent = 0;
	r->upstream->header_sent = 0

	r->header_hash = 1;
	return NGX_OK;
}

/**
	The main utility only deal with http status code.
**/
ngx_int_t lupstream_process_header(ngx_http_request_t *r){
	u = r->upstream;
	rc = ngx_http_parse_status_line(r, &u->buffer, &ctx->status);

	if (u->state){
		u->state->status = ctx->status.code;
	}

	u->headers_in.status_n = ctx->status.code;
	u->headers_in.status_line.len = ctx->status.end - ctx->status.start;
	if (u->headers_in.status_line.data == NULL){
		return NGX_ERROR;
	}

	//TODO:process header.
}
