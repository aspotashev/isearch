/* http://www.rubyinside.com/how-to-create-a-ruby-extension-in-c-in-under-5-minutes-100.html */

#include "ruby.h"

#include "../lib.h"

void init_search(const char *f_dump, const char *f_index, const char *f_map);
const char *find_string_id_by_str_multiple(char *s, int n);

//-------------------------------

VALUE wrap_init_search(VALUE self, VALUE dump, VALUE index, VALUE map)
{
	init_search(StringValuePtr(dump), StringValuePtr(index), StringValuePtr(map));
	return Qnil;
}

VALUE wrap_find_string_id(VALUE self, VALUE str, VALUE num)
{
	int n = FIX2INT(num);

	VALUE res = rb_ary_new();
	for (int index = get_internal_index_by_string(StringValuePtr(str)), i = 0;
		i < n; index ++, i ++)
	{
		rb_ary_push(res, rb_str_new2(get_msg_id_by_internal_index(index)));
	}
	return res;
}

extern "C" {

/* Function called at module loading */
void Init_isearch()
{
	VALUE IndexSearch = rb_define_module("IndexSearch");
	rb_define_singleton_method(IndexSearch, "init", RUBY_METHOD_FUNC(wrap_init_search), 3);
	rb_define_singleton_method(IndexSearch, "find", RUBY_METHOD_FUNC(wrap_find_string_id), 2);
}

}

