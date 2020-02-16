
struct ai_type
{
	void				*controller ;
	AIN_DeviceHandle	*handle;
	struct MsgPort		*msgport;
	int id;
	ULONG connected;
};

extern int joy_input(struct IOStdReq *kio);

