struct S_Control
{
	int SendType;
	C_StringList SL_Header,SL_Routeto,SL_Path,SL_ToWrite,SL_Via;
	S_FQAddress ShouldGo,OurAKA;
	int GotSystem;
	int AttribPack;
	char ext[4],extattach[4];
	char savepath[256],savepathattach[256],define[256];
        int is_qqq;
};
