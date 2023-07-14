#ifndef _HTTPS_REQ_H_
#define _HTTPS_REQ_H_

#define MAX_REQUEST_BUF_LEN 512

extern char https_req_buf[MAX_REQUEST_BUF_LEN];

void https_request_task(void *pvParameters);

#endif /* _HTTPS_REQ_H_ */