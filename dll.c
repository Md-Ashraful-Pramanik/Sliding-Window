#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/***************** Start Color Printing **************************/
#ifdef __unix__
#define OS UNIX
#define RESET_COLOR 0
#define NORMAL_COLOR 100
#endif
#ifdef _WIN32
#include <windows.h>
HANDLE  hConsole;
#define RESET_COLOR 7
#define NORMAL_COLOR 224
#endif

void resetColor() {
#ifdef __unix__
    printf("\e[0m");
#endif
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, RESET_COLOR);
#endif
    printf("\n");
}

void printColor(char *str, int windowsColorNumber, int linuxColorNumber, int linuxColorNumber2) {
#ifdef __unix__
    printf("\e[0m");
    printf("\e[%d;%dm %-100s", linuxColorNumber, linuxColorNumber2, str);
    printf("\e[0m");
    printf("\e[1;%dm", NORMAL_COLOR);
#endif
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, windowsColorNumber);
    printf("%-70s", str);
    SetConsoleTextAttribute(hConsole, NORMAL_COLOR);
#endif
    printf("\n");
}

// void printInfo(char *str) {
//     printColor(str, 112, 7, 93);
// }
//
// void printSuccess(char *str) {
//     printColor(str, 47, 7, 92);
// }
//
// void printActivity(char *str) {
//     printColor(str, 63, 7, 94);
// }
//
// void printError(char *str) {
//     printColor(str, 79, 6, 41);
// }


void printInfo(char *str) {
    printColor(str, 112, 7, 93);
}

void printSuccess(char *str) {
    printColor(str, 47, 6, 42);
}

void printActivity(char *str) {
    printColor(str, 63, 6, 44);
}

void printError(char *str) {
    printColor(str, 79, 6, 41);
}


/***************** End Color Printing **************************/


/* ./a.out | aha --black --title 'dll' > dll.html */

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
       are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1 /* change to 1 if you're doing extra credit */
/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_layer1 1
#define FROM_LAYER1 2

#define OFF 0
#define ON 1
#define A 0
#define B 1

#define DATA_FRAME_TYPE 0
#define ACK_FRAME_TYPE 1
#define PIGGYBACKED_TYPE 2

#define true 1
#define false 0
#define bool char

#define MAX_SEQ_NUM 2
#define PKT_DATA_SIZE 4
#define RETRANSMIT_RATE 10
#define NEXT_SEQ_NUM(n) (((n)+1)%MAX_SEQ_NUM)

int show_crc_steps = 0;
int use_piggybacked_ack = 0;
char generator_polynomial[100];

/* a "pkt" is the data unit passed from layer 3 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 3 via the students transport level protocol entities.         */
struct pkt {
    char data[PKT_DATA_SIZE];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct frm {
    char type;
    char seq_num;
    char ack_num;
    char payload[PKT_DATA_SIZE];

    int checksum;
};

struct sender {
    char current_seq_num;
    char previous_seq_num;

    char last_ack_num;
    int total_ack_received;

    struct frm frame;
};

struct receiver {
    char last_successful_seq_num;

    char outstanding_ack;
    char pending_ack_num;

    int total_frame_received;
};

struct status {
    struct sender *s;
    struct receiver *r;
};

struct status *status_A;
struct status *status_B;

/***************** Start CRC potion **************************/

char *get_CRC(char *input) {
    int generator_length = strlen(generator_polynomial);
    for (int i = 0; i < (generator_length - 1); ++i)
        strcat(input, "0");

    int input_length = strlen(input);

    char *remainder = (char *) malloc(generator_length * sizeof(char));
    for (int i = 0; i < (generator_length - 1); ++i)
        remainder[i + 1] = input[i];

    for (int i = generator_length; i <= input_length; ++i) {
        for (int j = 1; j < generator_length; ++j)
            remainder[j - 1] = remainder[j];

        remainder[generator_length - 1] = input[i - 1];

        if (remainder[0] == generator_polynomial[0]) {
            for (int j = 0; j < generator_length; ++j) {
                if (remainder[j] == generator_polynomial[j])
                    remainder[j] = '0';
                else
                    remainder[j] = '1';
            }
        }
    }

    for (int j = 1; j < generator_length; ++j)
        remainder[j - 1] = remainder[j];

    remainder[generator_length - 1] = '\0';
    input[input_length - generator_length + 1] = '\0';

    return remainder;
}

char *char_to_bit_string(char c) {
    char *str = (char *) malloc(sizeof(char) * 9);
    for (int i = 0; i < 8; ++i) {
        if (c & (1 << (7 - i)))
            str[i] = '1';
        else
            str[i] = '0';
    }
    str[8] = '\0';
    return str;
}

int string_to_int(char *str) {
    int length = strlen(str);
    int result = 0;
    for (int i = 0; i < length; ++i) {
        if (str[i] == '1')
            result |= (1 << (length - 1 - i));
    }
    return result;
}

int get_check_sum(struct frm frame) {

    char *input = (char *) malloc(sizeof(char) * 100);
    strcat(input, "");

    char *str;

    str = char_to_bit_string(frame.type);
    strcat(input, str);
    free(str);

    str = char_to_bit_string(frame.seq_num);
    strcat(input, str);
    free(str);

    str = char_to_bit_string(frame.ack_num);
    strcat(input, str);
    free(str);

    for (int i = 0; i < PKT_DATA_SIZE; ++i) {
        str = char_to_bit_string(frame.payload[i]);
        strcat(input, str);
        free(str);
    }

    char *rem = get_CRC(input);
    int remainder = string_to_int(rem);

    if (show_crc_steps) {
        printInfo("%%%%%%  In CRC calculation: ");
        char s[200];
        sprintf(s, "%%%%%%  Input Bit String: %s", input);
        printActivity(s);
        sprintf(s, "%%%%%%  Generator polynomial: %s", generator_polynomial);
        printActivity(s);
        sprintf(s, "%%%%%%  Calculated CRC: %s", rem);
        printActivity(s);
    }

    free(input);
    free(rem);

    return remainder;
}

/***************** End CRC potion **************************/


/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void starttimer(int AorB, float increment);

void stoptimer(int AorB);

void tolayer1(int AorB, struct frm frame);

void tolayer3(int AorB, char packet[PKT_DATA_SIZE]);

void output(struct pkt packet, int AorB);

void input(struct frm frame, int AorB);

void process_ack_frame(struct frm frame, int AorB);

void process_data_frame(struct frm frame, int AorB);

void send_ack_or_nack(struct frm frame, int AorB, int isACK);

void timer_interrupt(int AorB);

struct status *get_new_status();

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

struct status *get_status(int AorB) {
    if (AorB == A) return status_A;
    else return status_B;
}

/* called from layer 3, passed the data to be sent to other side */
void A_output(struct pkt packet) {
    printInfo("In A_output");
    output(packet, A);
}

/* need be completed only for extra credit */
void B_output(struct pkt packet) {
    printInfo("In B_output");
    output(packet, B);
}

void output(struct pkt packet, int AorB) {
    struct status *stat = get_status(AorB);
    if (stat->s->previous_seq_num != stat->s->last_ack_num) {
        printError("***Drop a packet. Because previous message is transmitting.");
        return;
    }

    stat->s->frame.seq_num = stat->s->current_seq_num;
    strcpy(stat->s->frame.payload, packet.data);

    if (use_piggybacked_ack && stat->r->outstanding_ack) {
        stat->s->frame.ack_num = stat->r->pending_ack_num;
        stat->s->frame.type = PIGGYBACKED_TYPE;
        stat->r->outstanding_ack = 0;
        printActivity("Sending Piggybacked acknowledgement");
    } else {
        stat->s->frame.ack_num = -1;
        stat->s->frame.type = DATA_FRAME_TYPE;
    }

    stat->s->frame.checksum = get_check_sum(stat->s->frame);

    stat->s->previous_seq_num = stat->s->current_seq_num;
    stat->s->current_seq_num = NEXT_SEQ_NUM(stat->s->current_seq_num);

    printActivity("Sending a new Packet.");
    tolayer1(AorB, stat->s->frame);
    printActivity("Start a timer");
    starttimer(AorB, RETRANSMIT_RATE);
}

/* called from layer 1, when a packet arrives for layer 2 at A*/
void A_input(struct frm frame) {
    printInfo("In A_input");
    input(frame, A);
}

/* called from layer 1, when a packet arrives for layer 2 at B*/
void B_input(struct frm frame) {
    printInfo("In B_input");
    input(frame, B);
}

void input(struct frm frame, int AorB) {
    int checkSum = get_check_sum(frame);

    if (checkSum != frame.checksum) {
        printError("Corrupted packet received.");
        send_ack_or_nack(frame, AorB, 0);
        return;
    }

    // printf("%d\n", frame.type);

    if (frame.type == DATA_FRAME_TYPE) {
        printActivity("Get a Data frame.");
        process_data_frame(frame, AorB);
    } else if (frame.type == ACK_FRAME_TYPE) {
        printActivity("Get a ACK frame.");
        process_ack_frame(frame, AorB);
    } else if (frame.type == PIGGYBACKED_TYPE) {
        printActivity("Get a Piggybacked frame.");
        process_data_frame(frame, AorB);
        process_ack_frame(frame, AorB);
    }
}


void process_ack_frame(struct frm frame, int AorB) {
    struct status *stat = get_status(AorB);
    if (stat->s->previous_seq_num == stat->s->last_ack_num) {
        printActivity("Ignore this ack because already get ack.");
    } else if (frame.ack_num == frame.seq_num || frame.type == PIGGYBACKED_TYPE) {
        stat->s->total_ack_received++;
        char str[200];
        sprintf(str, "%s%d", "Get Ack Successfully. Total Acknowledgment: ", stat->s->total_ack_received);
        printSuccess(str);

        stat->s->last_ack_num = frame.ack_num;
        printActivity("Stop Timer");
        stoptimer(AorB);
    } else
        printError("Get a Negative ACK.");
}

void process_data_frame(struct frm frame, int AorB) {
    struct status *stat = get_status(AorB);
    if (frame.seq_num == stat->r->last_successful_seq_num) {
        printActivity("Ignore this frame because already get this message.");
    } else {
        stat->r->total_frame_received++;
        char str[200];
        sprintf(str, "%s%d", "Get Frame Successfully, Total Successful: ", stat->r->total_frame_received);
        printSuccess(str);

        stat->r->outstanding_ack = 0;

        printSuccess("Sending Packet to layer3");
        stat->r->last_successful_seq_num = frame.seq_num;
        tolayer3(AorB, frame.payload);
    }

    send_ack_or_nack(frame, AorB, 1);
}

void send_ack_or_nack(struct frm frame, int AorB, int isACK) {
    struct status *stat = get_status(AorB);
    if (!use_piggybacked_ack || stat->r->outstanding_ack) {
        stat->r->outstanding_ack = 0;

        frame.type = ACK_FRAME_TYPE;

        /// ignoring payload in ack/nack
        for (int i = 0; i < PKT_DATA_SIZE; ++i)
            frame.payload[i] = 0;

        if (isACK) {
            printActivity("Sending ACK");
            frame.ack_num = frame.seq_num;
        } else {
            printActivity("Sending NACK");
            frame.ack_num = stat->r->last_successful_seq_num;
            frame.seq_num = NEXT_SEQ_NUM(frame.ack_num);
        }

        frame.checksum = get_check_sum(frame);
        tolayer1(AorB, frame);
    } else if (isACK) {
        printActivity("Storing ACK for Piggybacking");
        stat->r->outstanding_ack = 1;
        stat->r->pending_ack_num = frame.seq_num;
    }
}

/* called when A's timer goes off */
void A_timer_interrupt() {
    printInfo("In A_timer_interrupt");
    timer_interrupt(A);
}

/* called when B's timer goes off */
void B_timer_interrupt() {
    printInfo("In B_timer_interrupt");
    timer_interrupt(B);
}

void timer_interrupt(int AorB) {
    struct status *stat = get_status(AorB);
    if (stat->s->previous_seq_num == stat->s->last_ack_num) {
        printActivity("Ignore this interrupt because already get ack.");
        return;
    }
    //printf("Retransmit from timer interrupt.");
    printActivity("Retransmit Packet");
    if (stat->r->outstanding_ack) {
        stat->s->frame.ack_num = stat->r->pending_ack_num;
        stat->s->frame.type = PIGGYBACKED_TYPE;
        stat->r->outstanding_ack = 0;
        printActivity("Sending Piggybacked acknowledgement");
    } else {
        stat->s->frame.ack_num = -1;
        stat->s->frame.type = DATA_FRAME_TYPE;
    }
    stat->s->frame.checksum = get_check_sum(stat->s->frame);
    tolayer1(AorB, stat->s->frame);
    starttimer(AorB, RETRANSMIT_RATE);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
    printInfo("In A_init");
    status_A = get_new_status();
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init() {
    printInfo("In B_init");
    status_B = get_new_status();
}

struct status *get_new_status() {
    struct status *stat = (struct status *) malloc(sizeof(struct status));
    stat->s = (struct sender *) malloc(sizeof(struct sender));
    stat->r = (struct receiver *) malloc(sizeof(struct receiver));

    stat->s->current_seq_num = 0;
    stat->s->last_ack_num = -1;
    stat->s->previous_seq_num = -1;
    stat->s->total_ack_received = 0;

    stat->r->total_frame_received = 0;

    stat->r->outstanding_ack = 0;
    stat->r->pending_ack_num = -1;

    stat->r->last_successful_seq_num = -1;

    return stat;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 1 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and packet loss) of packets across the layer 1/4 interface
    - handles the starting/stopping of a timer, and generates timer
        interrupts (resulting in calling students timer handler).
    - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event {
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct frm *frmptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of messages from 5 to 4 so far */
int nsimmax = 0;   /* number of frames to generate, then stop */
float time = 0.000;
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 3 */
int ntolayer1;     /* number sent into layer 1 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();

void generate_next_arrival(void);

void insertevent(struct event *p);

int main() {
    //setbuf(stdout, NULL);
    /********************Change Start***************************/
#ifdef _WIN32
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE); /// Added for coloring
#endif
    printColor(" ", NORMAL_COLOR, NORMAL_COLOR, 6);
    /********************Change End***************************/

    struct event *eventptr;
    struct pkt pkt2give;
    struct frm frm2give;

    int i, j;
    char c;

    /************ Start taking Additional Input ****************/
    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    int temp;
    printf("CRC steps: ");
    scanf("%d", &temp);
    if (temp == 1)
        show_crc_steps = 1;

    printf("Piggybacking: ");
    scanf("%d", &temp);
    if (temp == 1)
        use_piggybacked_ack = 1;

    printf("Generator polynomial: ");
    scanf("%s", generator_polynomial);

    /************ End taking Additional Input ****************/

    init();
    A_init();
    B_init();

    while (1) {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2) {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer3 ");
            else
                printf(", fromlayer1 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (eventptr->evtype == FROM_layer1) {
            if (nsim < nsimmax) {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in pkt to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < PKT_DATA_SIZE; i++)
                    pkt2give.data[i] = 97 + j;
                //pkt2give.data[] = 0;
                if (TRACE > 2) {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < PKT_DATA_SIZE; i++)
                        printf("%c", pkt2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(pkt2give);
                else
                    B_output(pkt2give);
            }
        } else if (eventptr->evtype == FROM_LAYER1) {
            frm2give.seq_num = eventptr->frmptr->seq_num;
            frm2give.ack_num = eventptr->frmptr->ack_num;
            frm2give.checksum = eventptr->frmptr->checksum;
            frm2give.type = eventptr->frmptr->type;
            for (i = 0; i < PKT_DATA_SIZE; i++)
                frm2give.payload[i] = eventptr->frmptr->payload[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(frm2give); /* appropriate entity */
            else
                B_input(frm2give);
            free(eventptr->frmptr); /* free the memory for packet */
        } else if (eventptr->evtype == TIMER_INTERRUPT) {
            if (eventptr->eventity == A)
                A_timer_interrupt();
            else
                B_timer_interrupt();
        } else {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

    terminate:
    printf(" Simulator terminated at time %f\n after sending %d frames from layer3\n",
           time, nsim);


    /********************Change Start***************************/
    printSuccess("Statistics");
    char str[200];
    sprintf(str, "A sends %d frames successfully.", status_A->s->total_ack_received);
    printSuccess(str);
    sprintf(str, "A receives %d frames successfully.", status_A->r->total_frame_received);
    printSuccess(str);
    sprintf(str, "B sends %d frames successfully.", status_B->s->total_ack_received);
    printSuccess(str);
    sprintf(str, "B receives %d frames successfully.", status_B->r->total_frame_received);
    printSuccess(str);

    free(status_A->s);
    free(status_A->r);
    free(status_B->s);
    free(status_B->r);
    free(status_A);
    free(status_B);
    resetColor(); /// Added for coloring (RESET COLOR)
    /********************Change End***************************/
    return 0;
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("Enter the number of messages to simulate: ");
    scanf("%d", &nsimmax);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f", &lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f", &corruptprob);
    printf("Enter average time between messages from sender's layer3 [ > 0.0]:");
    scanf("%f", &lambda);
    printf("Enter TRACE:");
    scanf("%d", &TRACE);

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer1 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;              /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void) {
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void) {
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_layer1;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p) {
    struct event *q, *qold;

    if (TRACE > 2) {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;      /* q points to header of list in which p struct inserted */
    if (q == NULL)   /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    } else {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL)   /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        } else if (q == evlist)     /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        } else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void) {
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next) {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB /* A or B is trying to stop timer */) {
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist)   /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            } else     /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start timer */, float increment) {
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** tolayer1 ***************/
void tolayer1(int AorB, struct frm packet) {
    struct frm *myfrmptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer1++;

    /* simulate losses: */
    if (jimsrand() < lossprob) {
        nlost++;
        if (TRACE > 0)
            printError("          tolayer1: packet being lost");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    myfrmptr = (struct frm *) malloc(sizeof(struct frm));
    myfrmptr->seq_num = packet.seq_num;
    myfrmptr->ack_num = packet.ack_num;
    myfrmptr->checksum = packet.checksum;
    myfrmptr->type = packet.type;
    for (i = 0; i < PKT_DATA_SIZE; i++)
        myfrmptr->payload[i] = packet.payload[i];
    if (TRACE > 2) {
        printf("          tolayer1: seq: %d, ack %d, check: %d ", myfrmptr->seq_num,
               myfrmptr->ack_num, myfrmptr->checksum);
        for (i = 0; i < PKT_DATA_SIZE; i++)
            printf("%c", myfrmptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER1;      /* packet will pop out from layer1 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->frmptr = myfrmptr;         /* save ptr to my copy of packet */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER1 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob) {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            myfrmptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            myfrmptr->seq_num = 213;
        else
            myfrmptr->ack_num = 213;
        if (TRACE > 0)
            printError("          tolayer1: packet being corrupted");
    }

    if (TRACE > 2)
        printf("          tolayer1: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer3(int AorB, char packet[PKT_DATA_SIZE]) {
    int i;
    if (TRACE > 2) {
        printf("          tolayer3: data received: ");
        for (i = 0; i < PKT_DATA_SIZE; i++)
            printf("%c", packet[i]);
        printf("\n");
    }
}
