// $Id$

#ifndef _TRACE_H_
#define _TRACE_H_

#define tracev(var)															\
			trace("%s: 0x%x\n", #var, (int) var)							\

#define trace(...)															\
			fprintf(stderr, __VA_ARGS__)									\

#define trace_fn(...)														\
			fprintf(stderr, "%s\n", __func__)								\

#define hexdisplay(str, buf, len)											\
		{																	\
			int i;															\
			trace("%s (%d)", str, len);										\
			for (i = 0; i < len; ++i) {										\
				if (i % 16 == 0) {											\
					trace("\n");											\
				}															\
				trace("%02x ", buf[i]);										\
			}																\
			trace("\n");													\
		}																	\

#endif

