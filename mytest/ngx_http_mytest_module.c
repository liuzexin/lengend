/*
** Copy Right  CoderLiu(github)
**
*/
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t ngx_http_mytest_commands[] = {
	{ngx_string("mytest"),
	 NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
     ngx_http_mytest,
	 0,
	 0,
     NULL},
	ngx_null_command
};

static ngx_http_module_t ngx_http_mytest_module_ctx = {
	
	NULL,
	NULL,
	
	NULL,
	NULL,
	
	NULL,
	NULL,
	
    NULL,
    NULL
};


ngx_module_t ngx_http_mytest_module = {
	NGX_MODULE_V1,
	&ngx_http_mytest_module_ctx,
	ngx_http_mytest_commands,
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

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t * r){
	ngx_log_error(NGX_LOG_WARN, r->connection->log, 0,
                   "http mytest: \"%s\"", (char *)"Test");

    r->headers_out.status=NGX_HTTP_OK;
    //ngx_chain_t   out;
    //out.next = NULL;
    ngx_str_t  *ct;
    ct->data = (u_char *)"applicaiton/json";
    ctx->len = sizeof("applicaiton/json") - 1;
    ngx_str_t data;
    data.data = (u_char *)"{data:\"Hello world\"}";
    data.len = sizeof("{data:\"Hello world\"}") - 1;
    ngx_http_complex_value_t val = {
    	data,
    	NULL,
    	NULL,
    	NULL
    };
    ngx_http_send_response(r, (ngx_uint_int)200, ct, &val);
	return NGX_OK;
}


/**
 * copy right: http://nutrun.com/weblog/2009/08/15/hello-world-nginx-module.html
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
      ngx_buf_t    *b;
        
          r->headers_out.content_type.len = sizeof("text/plain") - 1;
            r->headers_out.content_type.data = (u_char *) "text/plain";
            
              b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
              
                out.buf = b;
                  out.next = NULL;
                  
                    b->pos = ngx_hello_world;
                      b->last = ngx_hello_world + sizeof(ngx_hello_world);
                        b->memory = 1;
                          b->last_buf = 1;
                          
                            r->headers_out.status = NGX_HTTP_OK;
                              r->headers_out.content_length_n = sizeof(ngx_hello_world);
                                ngx_http_send_header(r);
                                
                                  return ngx_http_output_filter(r, &out);
}
*/
static char* ngx_http_mytest(ngx_conf_t *cf, ngx_command_t*cmd, void *conf){
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_mytest_handler;
	return NGX_CONF_OK;
}

