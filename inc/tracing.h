/* tracing.h */
#ifndef __TRACING__
#define __TRACING__

#include <stdint.h>
#define EVENT_BUF_MAX (20480)
#define EVENT_NAME_LEN (40)

#define HOST_EVENT    (1)
#define GUEST_EVENT   (2)
#define TASK_EVENT    (3)

/* ============== HOST EVENT =============== */
#define HOST_EVENT_RCV_CHGREQ	  (101)
#define HOST_EVENT_ATT_CHGEND     (102)
#define HOST_EVENT_SND_UPWIACK    (103)
#define HOST_EVENT_SND_UPWOACK    (104)
#define HOST_EVENT_RCV_CHGWOACK   (105)
#define HOST_EVENT_RCV_CHGWIACK   (106)

#define GUEST_EVENT_RCV_UPWIACK   (201)
#define GUEST_EVENT_RCV_UPWOACK   (202)
#define GUEST_EVENT_SND_CHGWIACK  (203)
#define GUEST_EVENT_SND_CHGWOACK  (204)

#define TASK_EVENT_RELEASE_JOB    (301)
#define TASK_EVENT_FINISH_JOB	  (302)

#define HZ                 (3300000000)
#define KHZ                (3300000)
#define MHZ                (3300)

typedef int32_t EVENT_ID;
typedef uint8_t EVENT_TYPE;

typedef struct event_record_t
{
	EVENT_TYPE u16type;
	EVENT_ID s32id;
	int32_t  s32srcID;
	uint64_t u64tsc;
}event_record;

typedef struct event_identify_t
{
	char acEvent[EVENT_NAME_LEN];
	EVENT_ID s32id;
} event_identify;

void init_tracing(void);
__inline__ void add_record_host (EVENT_ID s32event);
__inline__ void add_record_guest(EVENT_ID s32event, int32_t s32RegID);
__inline__ void add_record_task (EVENT_ID s32event, int32_t s32RefID);
void dump_tracing_to_file(char* pcName);
void dump_tracing_to_screen(void);
char* getEventNamefromID(EVENT_ID s32ID);

#endif
