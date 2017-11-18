/*
** Copy Right  CoderLiu(github)
**
*/
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define LUS_CONNECTION_TIMEOUT 600;
static char *ngx_http_lupstrea(mngx_conf_t *cf, ngx_command_t *cmd, void *conf);

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
	ngx_log_error(NGX_LOG_WARN, r->connection->log, 0,
                   "http lupstream: \"%s\"", (char *)"Test");

    //r->headers_out.status=NGX_HTTP_OK;
    //ngx_chain_t   out;
    //out.next = NULL;
    ngx_uint_t  status = 200;
    ngx_str_t  ct = ngx_string("applicaiton/json");
    ngx_str_t data = ngx_string("{data:\"Hello world\"}");
    ngx_http_complex_value_t val = {
    	data,
    	NULL,
    	NULL,
    	NULL
    };
    ngx_http_send_response(r,  status, &ct, &val);
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

