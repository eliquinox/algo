
#include "algo.h"

static struct tcp_pcb *c_pcb;

// FIXME: sending time 
static char logon[] = "8=FIX.4.4\0019=93\00135=A\00149=trader\00156=exchange\00134=1\00152=20221001-12:29:18.923\00198=0\001108=30\001553=trader\001554=password\00110=112\001";
static char new_order_single[] = "8=FIX.4.4\0019=106\00135=D\00134=3\00149=trader\00156=exchange\00152=20221001-21:49:10.748\00111=limitBid\00155=BTCUSD\00140=2\00154=1\00144=10000.51\00138=1\00110=198\001";

static int client_connected = FALSE;
static int logon_received = FALSE;
static int nos_sent = FALSE;


void print_app_header()
{
	xil_printf("TCP client connecting to %s on port %d\r\n",
			TCP_SERVER_IP_ADDRESS, TCP_CONN_PORT);
}

static void print_tcp_conn_stats()
{
	xil_printf("Connected %s:%d with %s port %d\r\n", 
			inet_ntoa(c_pcb->local_ip),
			c_pcb->local_port, 
			inet_ntoa(c_pcb->remote_ip),
			c_pcb->remote_port);
}

/** Close a tcp session */
static void tcp_client_close(struct tcp_pcb *pcb)
{
	err_t err;

	if (pcb != NULL) {
		tcp_sent(pcb, NULL);
		tcp_err(pcb, NULL);
		err = tcp_close(pcb);
		if (err != ERR_OK) {
			/* Free memory with abort */
			tcp_abort(pcb);
		}
	}
}

/** Error callback, tcp session aborted */
static void tcp_client_err(void *arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
	// u64_t now = get_time_ms();
	// u64_t diff_ms = now - client.start_time;

	tcp_client_close(c_pcb);
	c_pcb = NULL;
	xil_printf("TCP connection aborted\n\r");
}

static err_t send_logon(void)
{
	err_t err;
	u8_t apiflags = TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE;

	if (c_pcb == NULL) {
		return ERR_CONN;
	}

#ifdef __MICROBLAZE__
	/* Zero-copy pbufs is used to get maximum performance for Microblaze.
	 * For Zynq A9, ZynqMP A53 and R5 zero-copy pbufs does not give
	 * significant improvement hense not used. */
	apiflags = 0;
#endif


	while (tcp_sndbuf(c_pcb) > TCP_SEND_BUFSIZE) 
	{
		err = tcp_write(c_pcb, logon, strlen(logon), apiflags);
		if (err != ERR_OK) {
			xil_printf("TCP client: Error on tcp_write: %d\r\n", err);
			return err;
		}

		err = tcp_output(c_pcb);
		if (err != ERR_OK) {
			xil_printf("TCP client: Error on tcp_output: %d\r\n", err);
			return err;
		}

		break;
	}
	
	return ERR_OK;
}


static err_t send_nos(void)
{
	err_t err;
	u8_t apiflags = TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE;

	if (c_pcb == NULL) {
		return ERR_CONN;
	}

#ifdef __MICROBLAZE__
	/* Zero-copy pbufs is used to get maximum performance for Microblaze.
	 * For Zynq A9, ZynqMP A53 and R5 zero-copy pbufs does not give
	 * significant improvement hense not used. */
	apiflags = 0;
#endif


	while (tcp_sndbuf(c_pcb) > TCP_SEND_BUFSIZE) 
	{
		err = tcp_write(c_pcb, new_order_single, strlen(new_order_single), apiflags);
		if (err != ERR_OK) {
			xil_printf("TCP client: Error on tcp_write: %d\r\n", err);
			return err;
		}

		err = tcp_output(c_pcb);
		if (err != ERR_OK) {
			xil_printf("TCP client: Error on tcp_output: %d\r\n", err);
			return err;
		}

		break;
	}
	
	return ERR_OK;
}




/** TCP sent callback */
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	xil_printf("In tcp_client_sent() Len: %d\n\r", len);
	return ERR_OK;
}

/** TCP received callback */
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	xil_printf("In tcp_client_recv(): %s\n\r", p -> payload);
	logon_received = TRUE;
	return ERR_OK;
}

/** TCP connected callback (active connection), send data now */
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	xil_printf("In tcp_client_connected() Err: %d\n\r", err);

	if (err != ERR_OK) {
		tcp_client_close(tpcb);
		xil_printf("Connection error\n\r");
		return err;
	}
	/* store state */
	c_pcb = tpcb;

	client_connected = TRUE;

	print_tcp_conn_stats();

	/* set callback values & functions */
	tcp_arg(c_pcb, NULL);
	tcp_sent(c_pcb, tcp_client_sent);
	tcp_recv(c_pcb, tcp_client_recv);
	tcp_err(c_pcb, tcp_client_err);

	return ERR_OK;
}

void do_work()
{
	if (client_connected && !logon_received)
	{
		xil_printf("About to send logon\r\n");
		send_logon();
		return;
	}
	
	if (logon_received && !nos_sent)
	{
		xil_printf("About to send NOS\r\n");
		send_nos();
		nos_sent = TRUE;
		return;
	}
}

void start_application(void)
{
	err_t err;
	struct tcp_pcb* pcb;
	ip_addr_t remote_addr;

	err = inet_aton(TCP_SERVER_IP_ADDRESS, &remote_addr);

	if (!err) {
		xil_printf("Invalid Server IP address: %d\r\n", err);
		return;
	}

	/* Create Client PCB */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error in PCB creation. out of memory\r\n");
		return;
	}

	err = tcp_connect(pcb, &remote_addr, TCP_CONN_PORT, tcp_client_connected);
	if (err) {
		xil_printf("Error on tcp_connect: %d\r\n", err);
		tcp_client_close(pcb);
		return;
	}

	return;
}
