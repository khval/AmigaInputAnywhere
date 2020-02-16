
#define max_files 1000
extern char *files[max_files];


extern void WriteConfig(const char *fname);
extern void make_cfg_files_list(struct List *list);
extern void get_number_array(char *txt,int *num);
extern void ReadConfig(char *fname);

