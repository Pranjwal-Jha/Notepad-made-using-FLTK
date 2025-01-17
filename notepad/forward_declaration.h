#pragma once
void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v);
void copy_cb(Fl_Widget*, void* v);
void delete_cb(Fl_Widget*, void* v);
void find2_cb(Fl_Widget* w, void* v);
void find_cb(Fl_Widget* w, void* v);
void new_cb(Fl_Widget*, void* v);
void open_cb(Fl_Widget*, void*);
void paste_cb(Fl_Widget*, void* v);
void quit_cb(Fl_Widget*, void* v);
void replace_cb(Fl_Widget*, void* v);
//void save_cb(void);
//void saveas_cb(void);
//int check_save(void);
int check_save(void* v);
void load_file(char* newfile, int ipos);
void set_title(Fl_Window* w);
//void save_file(char* newfile);
bool save_file(const char* filename, Fl_Text_Buffer* textbuffer);
bool save_as(Fl_Text_Buffer* textbuffer);
void save_cb(Fl_Widget*, void* data);
void saveas_cb(Fl_Widget*, void* data);