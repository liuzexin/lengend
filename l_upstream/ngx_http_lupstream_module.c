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

typedef struct  {
	ngx_http_status_t status;
	
} ngx_http_lupstream_ctx_t;

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

	if(ngx_http_upstream_create(r) != NGX_OK){
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
			"Lupstream :Create upstream error");
		return NGX_ERROR;
	}

	ngx_http_lupstream_conf_t *lcf = (ngx_http_lupstream_conf_t *)
	ngx_http_get_module_loc_conf(r, ngx_http_lupstream_module_t);
	ngx_http_upstream_t *u = r->upstream;
	u->conf = & lcf->us;
	u->buffering = lcf->us.buffering;

	u->resolved = (ngx_http_upstream_resolved_t *)ngx_pcalloc(r->pool, sizeof(ngx_http_upstream_resolved_t));
	if (u->resolved == NULL){
		gx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
			"Lupstream :Create upstream error %s", strerror(errno));
		return NGX_ERROR;
	}

	static struct sockaddr_in backendScokAddr;
	struct  hostent * pHost = gethostbyname((char *) "www.baidu.com");
	backendScokAddr.sin_family = AF_INET;
	backendScokAddr.sin_port = htons((in_port_t) 80);
	char * pDmsIP = inet_ntoa(* (struct in_addr *) (pHost->h_addr_list[0]));

	backendScokAddr.sin_addr.s_addr = inet_addr(pDmsIP);
	u->resolved->sockaddr = (struct sockaddr*) &backendScokAddr;
	u->resolved->socklen = sizeof(struct sockaddr_in);
	u->resolved->naddrs = 1;

	// ngx_http_lupstream_ctx_t * lt = ngx_http_get_module_ctx(r, ngx_http_lupstream_module);

    r->upstream->create_request = lupstream_create_request;
    r->upstream->finalize_request = lupstream_finalize_request;
    r->upstream->process_header = lupstream_process_header;
    r->main->count++;//ref counter
    ngx_http_upstream_init(r);
	return NGX_DONE;
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
	clcf = ngx_http_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_lupstream_handler;
	return NGX_CONF_OK;
}


static void lupstream_finalize_request(ngx_http_request_t *r, ngx_int_t rc){
	ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, 
		"LUPSTREAM");
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
static ngx_int_t lupstream_process_header(ngx_http_request_t *r){
	u = r->upstream;

	if (u->state){
		u->state->status = ctx->status.code;
	}

	u->headers_in.status_n = ctx->status.code;
	u->headers_in.status_line.len = ctx->status.end - ctx->status.start;
	if (u->headers_in.status_line.data == NULL){
		return NGX_ERROR;
	}

	//TODO:process header.
	ngx_table_elt_t *h;
	ngx_http_upstream_header_t *hh;

	for (;;){
		rc = ngx_parse_header_line(&r->upstream->headers_in.headers);
		if (rc == NGX_OK){
			h = ngx_list_push(&r->upstream->buffer, 1);
			if (h == NULL)
			{
				return NGX_ERROR;
			}
		}
		h->hash = r->header_hash;

		h->key.len = r->header_name_end - r->header_name_start;
		h->value.len = r->header_end - r->header_start;

		h->key.data = ngx_pcalloc(r->pool, h->key.len * 2 + 2 + h->value.len);
		if (h->key.data == NULL){
			return NGX_ERROR;
		}
		h->value.data = h->key.data + h->key.len + 1;
		h->lowcase_key = h->key.data + h->key.len + 1 + h->value.len + 1;

		ngx_memcpy(h->key.data, r->header_name_start, h->key.len);
		h->key.data[h->key.len] = '\0';
		ngx_memcpy(h->value.data, r->header_start, h->value.len);
		h->value.data[h->value.len] = '\0';
		
		if (h->key.len == r->lowcase_index){
			ngx_memcpy(h->lowcase_key, r->lowcase_header, h->key_len);
		}else{
			ngx_strlow(h->lowcase_key, h->key.data, h->key_len);
		}

		hh = ngx_hash_find(&umcf->headers_in_hash, h->hash,
			h->lowcase_key,h->key_len);

		if (hh && hh->handler(r, h, hh->offset) != NGX_OK){
			return NGX_ERROR;
		}
		continue;

		if(NGX_HTTP_PARSE_HEADER_DONE){

			if (r->upstream->headers_in.server == NULL)
			{
				h = ngx_list_push(&r->upstream->headers_in.headers);
				if (h == NULL){
					return NGX_ERROR;
				}
				h->hash = ngx_hash(ngx_hash(ngx_hash(ngx_hash(ngx_hash('s', 'e'), 'r'), 'v'), 'e'), 'r');
				ngx_str_set(&h->key, "Server");
				ngx_str_null(&h->value);
				h->lowcase_key = (u_char *)"server";
			}

			if (r->upstream->headers_in.date == NULL)
			{
				h = ngx_list_push(&r->upstream->headers_in.headers);
				if (h == NULL){
					return NGX_ERROR;
				}
				h->hash = ngx_hash(ngx_hash(ngx_hash('d', 'a'), 't'), 'e');
				ngx_str_set(&h->key, "Date");
				ngx_str_null(&h->value);
				h->lowcase_key = (u_char *)"date";
			}
			return NGX_OK;
		}

		if (rc == NGX_AGIN){
			return rc;
		}
		ngx_log_error(NGX_LOG_ERR, r->connection->log,
			0, 'invalid heaeder!');
		return NGX_HTTP_UPSTREAM_INVALID_HEADER;
	}
}
