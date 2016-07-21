// Diese Datei muss fuer Perfmon Benutzung included werden ...
// Sie enthaelt alle Prototypes ...

void	pa_clear_statistics			(void);
int		pa_add_counter				(int *index,char *description);
void	pa_start_counter			(int index);
void	pa_stop_counter				(int index);
void	pa_do_statistic				(char *filename);





