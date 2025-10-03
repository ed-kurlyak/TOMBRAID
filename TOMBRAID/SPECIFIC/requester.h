#pragma once

#include "types.h"

//#include <stdint.h>

void InitRequester(REQUEST_INFO *req);
void RemoveRequester(REQUEST_INFO *req);
int32_t DisplayRequester(REQUEST_INFO *req);
void SetRequesterHeading(REQUEST_INFO *req, const char *string);
void ChangeRequesterItem(REQUEST_INFO *req, int32_t idx, const char *string,
						 uint16_t flag);
void AddRequesterItem(REQUEST_INFO *req, const char *string, uint16_t flag);
void SetRequesterSize(REQUEST_INFO *req, int32_t max_lines, int16_t y);
