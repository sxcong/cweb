// http-server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "libevent2/util-internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <libevent2/event2/event.h>
#include <libevent2/event2/http.h>
#include <libevent2/event2/buffer.h>
#include <libevent2/event2/util.h>
#include <libevent2/event2/keyvalq_struct.h>
#include <libevent2/event2/event_struct.h>
#include <libevent2/event2/util.h>

#include <string>
#include <vector>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "AdvAPI32.lib")
#ifdef _DEBUG
#pragma comment(lib, "debug/event.lib")
#pragma comment(lib, "debug/event_core.lib")
#pragma comment(lib, "debug/event_extra.lib")
#pragma comment(lib, "debug/glogd.lib")
#else
#pragma comment(lib, "release/event.lib")
#pragma comment(lib, "release/event_core.lib")
#pragma comment(lib, "release/event_extra.lib")
#pragma comment(lib, "release/glog.lib")
#endif


#define GLOG_NO_ABBREVIATED_SEVERITIES 
#include "glog/logging.h"

#pragma warning( disable : 4996)


#include "IRESTful.h"
#include "CDBMgr.h"
#include "redisclient.h"


static char *
strsep(char **s, const char *del)
{
	char *d, *tok;
	//assert(strlen(del) == 1);
	if (!s || !*s)
		return NULL;
	tok = *s;
	d = strstr(tok, del);
	if (d) {
		*d = '\0';
		*s = d + 1;
	}
	else
		*s = NULL;
	return tok;
}

/*
 * @param always_decode_plus: when true we transform plus to space even
 *     if we have not seen a ?.
 */
static int
evhttp_decode_uri_internal(
	const char *uri, size_t length, char *ret, int always_decode_plus)
{
	char c;
	int i, j, in_query = always_decode_plus;

	for (i = j = 0; uri[i] != '\0'; i++) {
		c = uri[i];
		if (c == '?') {
			in_query = 1;
		}
		else if (c == '+' && in_query) {
			c = ' ';
		}
		else if (c == '%' && isxdigit((unsigned char)uri[i + 1]) &&
			isxdigit((unsigned char)uri[i + 2])) {
			char tmp[] = { uri[i + 1], uri[i + 2], '\0' };
			c = (char)strtol(tmp, NULL, 16);
			i += 2;
		}
		ret[j++] = c;
	}
	ret[j] = '\0';

	return (j);
}

/*
 * Helper function to parse out arguments in a query.
 * The arguments are separated by key and value.
 */

void http_parse_query(const char *uri, vector<HttpParam>& vecParam)
{
	char *line = NULL;
	char *argument;
	char *p;


	/* No arguments - we are done */
	if (strchr(uri, '?') == NULL)
		return;

	if ((line = _strdup(uri)) == NULL)
	{
		//event_err(1, "%s: strdup", __func__);
	}


	argument = line;

	/* We already know that there has to be a ? */
	strsep(&argument, "?");

	p = argument;
	while (p != NULL && *p != '\0') {
		char *key, *value, *decoded_value;
		argument = strsep(&p, "&");

		value = argument;
		key = strsep(&value, "=");
		if (value == NULL)
		{
			//goto error;
			break;
		}

		if ((decoded_value = (char*)malloc(strlen(value) + 1)) == NULL)
		{
			//event_err(1, "%s: malloc", __func__);
		}

		evhttp_decode_uri_internal(value, strlen(value), decoded_value, 1 /*always_decode_plus*/);

		//event_debug(("Query Param: %s -> %s\n", key, decoded_value));
		//evhttp_add_header_internal(headers, key, decoded_value);
		HttpParam param;
		param.key = key;
		param.value = decoded_value;
		vecParam.push_back(param);

		free(decoded_value);
	}

	free(line);
}


struct evhttp_uri {
	unsigned flags;
	char *scheme; /* scheme; e.g http, ftp etc */
	char *userinfo; /* userinfo (typically username:pass), or NULL */
	char *host; /* hostname, IP address, or NULL */
	int port; /* port, or zero */
	char *path; /* path, or "". */
	char *query; /* query, or NULL */
	char *fragment; /* fragment or NULL */
};
/* This callback gets invoked when we get any http request that doesn't match
 * any other callback.  Like any evhttp server callback, it has a simple job:
 * it must eventually call evhttp_send_error() or evhttp_send_reply().
 */


 /* Callback used for the /dump URI, and for every non-GET request:
  * dumps all information to stdout and gives back a trivial 200 ok */
static void
dump_request_cb(struct evhttp_request *req, void *arg)
{
	const char *cmdtype;
	struct evkeyvalq *headers;
	struct evkeyval *header;
	struct evbuffer *buf;

	switch (evhttp_request_get_command(req)) {
	case EVHTTP_REQ_GET: cmdtype = "GET"; break;
	case EVHTTP_REQ_POST: cmdtype = "POST"; break;
	case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
	case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
	case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
	case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
	case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
	case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
	case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
	default: cmdtype = "unknown"; break;
	}

	printf("Received a %s request for %s\nHeaders:\n",
		cmdtype, evhttp_request_get_uri(req));

	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header;
		header = header->next.tqe_next) {
		printf("  %s: %s\n", header->key, header->value);
	}

	buf = evhttp_request_get_input_buffer(req);
	puts("Input data: <<<");
	while (evbuffer_get_length(buf)) {
		int n;
		char cbuf[128];
		n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
		if (n > 0)
			(void)fwrite(cbuf, 1, n, stdout);
	}
	puts(">>>");

	evhttp_send_reply(req, 200, "OK", NULL);
}
struct event_base *base;

static void
http_handle(struct evhttp_request *req, void *arg)
{
	struct evbuffer *evb = NULL;
	//const char *docroot = (char*)arg;
	const char *uri = evhttp_request_get_uri(req);
	struct evhttp_uri *decoded = NULL;
	const char *path;
	char *decoded_path;
	char *whole_path = NULL;
//	size_t len;
	int fd = -1;
//	struct stat st;

	if (evhttp_request_get_command(req) != EVHTTP_REQ_GET
		&& evhttp_request_get_command(req) != EVHTTP_REQ_POST) {
		dump_request_cb(req, arg);
		return;
	}

	printf("Got a GET request for <%s>\n", uri);
	LOG(INFO) <<"request:"<<uri;

	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (!decoded) {
		printf("It's not a good URI. Sending BADREQUEST\n");
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	//printf(decoded->query);
	//printf(decoded->fragment);


		
	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (!path) path = "/";

	/* We need to decode it, to see what path the user really wanted. */
	decoded_path = evhttp_uridecode(path, 0, NULL);
	if (decoded_path == NULL)
		goto err;

	/* Don't allow any ".."s in the path, to avoid exposing stuff outside
	 * of the docroot.  This test is both overzealous and underzealous:
	 * it forbids aceptable paths like "/this/one..here", but it doesn't
	 * do anything to prevent symlink following." */
	if (strstr(decoded_path, ".."))
		goto err;

	{
		char* decodeUri = evhttp_decode_uri(uri);
		vector<HttpParam> vecParam;
		http_parse_query(decodeUri, vecParam);
		free(decodeUri);

		string param;

		if (evhttp_request_get_command(req) == EVHTTP_REQ_POST)
		{
			struct evbuffer *buf = evhttp_request_get_input_buffer(req);
			if (buf != NULL)
			{
				while (evbuffer_get_length(buf)) {
					int n;
					char cbuf[128] = { '\0' };
					n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
					if (n > 0)
					{
						param += cbuf;
					}
					
				}
			}
		}
	

		if (strcmp(path, "/quit") == 0)
		{
			event_base_loopexit(base, NULL);
			evhttp_send_reply(req, 200, "OK", NULL);
			goto done;
		}
		else
		{
			string out;
			IRESTful::Instance().setData(path, vecParam, param, out);//处理具体请求

			/* This holds the content we're sending. */
			evb = evbuffer_new();
			evbuffer_add(evb, out.c_str(), out.size());
			evhttp_send_reply(req, 200, "OK", evb);
			LOG(INFO) <<"response:"<<out.c_str();
			goto done;
		}
	}
	
err:
	evhttp_send_error(req, 404, "Document was not found");

done:
	if (decoded)
		evhttp_uri_free(decoded);
	if (decoded_path)
		free(decoded_path);
	if (whole_path)
		free(whole_path);
	if (evb)
		evbuffer_free(evb);
}
struct timeval lasttime;
int event_is_persistent;


static void
timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	struct timeval newtime, difference;
	struct event *timeout = (struct event*)arg;
	double elapsed;

	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec +
		(difference.tv_usec / 1.0e6);

	printf("timeout_cb called at %d: %.3f seconds elapsed.\n",
		(int)newtime.tv_sec, elapsed);
	lasttime = newtime;

	if (!event_is_persistent) {
		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 1;
		event_add(timeout, &tv);
	}

#if 1
	static int count = 0;
	count++;
	if (count == 10)
	{
		//event_base_loopexit(base, NULL);
		return;
	}
#else
#endif
}

static void FatalMessageDump(const char* data, int size)
{
	//_CRT_SECURE_NO_WARNINGS
	FILE* pf = fopen("crashdump.dmp", "rb");
	fwrite(data, size, 1, pf);
	fclose(pf);
}
int
main(int argc, char **argv)
{
	int nRetCode = 0;

	//1 日志相关设置
	google::InitGoogleLogging("cweb.exe");// 设置日志文件名中的"文件名"字段.
	google::SetLogDestination(google::GLOG_INFO, "./log/cweb_");
	FLAGS_logbufsecs = 0; //缓冲日志输出，默认为30秒，此处改为立即输出
	FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出
	FLAGS_max_log_size = 100;//最大日志大小为 100MB

	LOG(INFO) << "start up";
	google::InstallFailureSignalHandler();
	google::InstallFailureWriter(&FatalMessageDump);

	
	// 启动http server
	struct evhttp *http;
	struct evhttp_bound_socket *handle;

	ev_uint16_t port = 8082;
#ifdef _WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
#else
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return (1);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Couldn't create an event_base: exiting\n");
		return 1;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (!http) {
		fprintf(stderr, "couldn't create evhttp. Exiting.\n");
		return 1;
	}

	/* The /dump URI will dump all requests to stdout and say 200 ok. */
	//evhttp_set_cb(http, "/dump", dump_request_cb, NULL);

	/* We want to accept arbitrary requests, so we need to set a "generic"
	 * cb.  We can also add callbacks for specific paths. */
	evhttp_set_gencb(http, http_handle, NULL);
	evhttp_set_timeout(http, 120);

	/* Now we tell the evhttp what port to listen on */
	handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", port);
	if (!handle) {
		fprintf(stderr, "couldn't bind to port %d. Exiting.\n",
			(int)port);
		return 1;
	}

	{
		//3 显示服务的IP和端口
		/* Extract and display the address we're listening on. */
		struct sockaddr_storage ss;
		evutil_socket_t fd;
		ev_socklen_t socklen = sizeof(ss);
		char addrbuf[128];
		void *inaddr;
		const char *addr;
		int got_port = -1;
		fd = evhttp_bound_socket_get_fd(handle);
		memset(&ss, 0, sizeof(ss));
		if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
			perror("getsockname() failed");
			return 1;
		}
		if (ss.ss_family == AF_INET) {
			got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
			inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
		}
		else {
			fprintf(stderr, "Weird address family %d\n",
				ss.ss_family);
			return 1;
		}
		addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf,
			sizeof(addrbuf));
		if (addr) {
			char uri_root[512];
			printf("Listening on %s:%d\n", addr, got_port);
			evutil_snprintf(uri_root, sizeof(uri_root),
				"http://%s:%d", addr, got_port);
		}
		else {
			fprintf(stderr, "evutil_inet_ntop failed\n");
			return 1;
		}
	}

	//4 定时器
	/* Initalize one event */
	{
		struct event timeout;
		struct timeval tv;
		//struct event_base *base;
		int flags;
		event_is_persistent = 1;
		flags = EV_PERSIST;

		event_assign(&timeout, base, -1, flags, timeout_cb, (void*)&timeout);

		evutil_timerclear(&tv);
		tv.tv_sec = 5;
		event_add(&timeout, &tv);

		evutil_gettimeofday(&lasttime, NULL);
	}

	//5 启动数据库
	CDBMgr::Instance().open();

	string test = "test";
#if 0
	//写入测试数据
	int t = time(NULL);
	CDBMgr::Instance().addAlarmRecord(1, t, (const unsigned char*)test.c_str(), test.size());
	//读取测试数据
	int count = 0;
	map<int, string> mapOut;
	count = CDBMgr::Instance().getAlarmRecord(1, 0, t + 100, mapOut);
#endif

#if 1
	RedisClient::Instance().connect();
	RedisClient::Instance().setString("1", "test1");
	string value;
	RedisClient::Instance().getString("1", value);
	RedisClient::Instance().disconnect();
#endif

	CDBMgr::Instance().close();


	//6 进入循环
	event_base_dispatch(base);
	
	//7 程序结束，清理
	evhttp_free(http);
	event_base_free(base);
	

	google::ShutdownGoogleLogging();
	return 0;
}
