// WebSpider.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//#include"curl.h"
//#include"gumbo.h"
#include"Funcions.h"

#include<assert.h>
#include "TemplateDelegate.h"







class TestClass
{
public:
	TestClass() {}
	virtual ~TestClass() {}
	void click(int pos) {
		printf("call back!");
	}
};

int main() {
	EventPool pool;
	TestClass  test;
	pool.Attach<TestClass>(&test, &(TestClass::click));
	pool.Invoke(1);

	return 0;
}
//static std::string strHtml;
//size_t write_cb(void* ptr,size_t size,size_t nmemb)
//{
//	strHtml.append((char*)ptr, size * nmemb);
//	return size * nmemb;
//}
//
//static const char* find_title(const GumboNode* root) {
//	assert(root->type == GUMBO_NODE_ELEMENT);
//	assert(root->v.element.children.length >= 2);
//
//	const GumboVector* root_children = &root->v.element.children;
//	GumboNode* head = NULL;
//	for (int i = 0; i < root_children->length; ++i) {
//		GumboNode* child = static_cast<GumboNode*>(root_children->data[i]);
//		if (child->type == GUMBO_NODE_ELEMENT &&
//			child->v.element.tag == GUMBO_TAG_HEAD) {
//			head = child;
//			break;
//		}
//	}
//	assert(head != NULL);
//
//	GumboVector* head_children = &head->v.element.children;
//	for (int i = 0; i < head_children->length; ++i) {
//		GumboNode* child = static_cast<GumboNode*>(head_children->data[i]);
//		if (child->type == GUMBO_NODE_ELEMENT &&
//			child->v.element.tag == GUMBO_TAG_TITLE) {
//			if (child->v.element.children.length != 1) {
//				return "<empty title>";
//			}
//			GumboNode* title_text = static_cast<GumboNode*>(child->v.element.children.data[0]);
//			assert(title_text->type == GUMBO_NODE_TEXT || title_text->type == GUMBO_NODE_WHITESPACE);
//			return title_text->v.text.text;
//		}
//	}
//	return "<no title found>";
//}
//
//void search_for_tag(GumboNode* node)
//{
//	if (node->type != GUMBO_NODE_ELEMENT)
//	{
//		if(node->type == GUMBO_NODE_TEXT || node->type == GUMBO_NODE_WHITESPACE)
//			std::cout << Functions::UTF8ToUnicode(node->v.text.text) << std::endl;
//		return;
//	}
//	//GumboAttribute* href;
//	//if (node->v.element.tag == GUMBO_TAG_A &&
//	//	(href = gumbo_get_attribute(&node->v.element.attributes,"href")))
//	//{
//	//	std::cout << href->value << std::endl;
//	//}
//
//	//if (node->v.element.tag == GUMBO_TAG_A)
//	//{
//	//	std::cout << Functions::UTF8ToUnicode(node->v.text.text) << std::endl;
//	//}
//
//	GumboVector* children = &node->v.element.children;
//	for (size_t i = 0; i < children->length; i++)
//	{
//		search_for_tag(static_cast<GumboNode*>(children->data[i]));
//	}
//}
//int main()
//{
//	CURL* curl = curl_easy_init();
//	CURLcode curl_code;
//	if (NULL == curl)
//	{
//		return 0;
//	}
//
//	curl_easy_setopt(curl, CURLOPT_URL, "www.sina.com.cn");
//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
//	curl_code = curl_easy_perform(curl);
//	if (curl_code != CURLE_OK)
//	{
//		std::cout << curl_easy_strerror(curl_code) << std::endl;
//	}
//
//	GumboOutput* output = gumbo_parse(strHtml.c_str());
//	search_for_tag(output->root);
//	//std::cout << Functions::UTF8ToUnicode(find_title(output->root)).c_str() << std::endl;
//	gumbo_destroy_output(&kGumboDefaultOptions, output);
//	curl_easy_cleanup(curl);
//	while (true)
//	{
//
//	}
//    return 0;
//}

