// 采用CURLOPT_WRITEFUNCTION 实现网页下载保存功能
// 采用CURLOPT_WRITEFUNCTION 实现网页下载保存功能
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>
//#include <regex>
#include <boost/regex.hpp>  
using namespace std;
 
struct decodeDate_T
{
    char position[128];         //地点
    //char feedbackRate[256];   
    char company[128];          //公司名称
    char salary[128];           //薪酬
    char place[128];            //工作地点
    char experience[32];        //经验年限
    char number[32];            //招聘人数
    char education[32];         //学历要求
    char positiontype[128];     //职位类别
    char nature[32];            //工作性质
    char info[1024*10];         //职位描述
    char source[32];            //采集来源
    char code[32];              //职位编码
    //char data                 //采集时间由数据库自动生成
};

char sj[][8]=
{
    "044",      //高级软件工程师
    "045",      //软件工程师
    "079",
    "665",
    "667",
    "668",
    "047",
    "048",
    "053",
    "679",
    "687",      //嵌入式软件开发
    "863",      //移动互联网开发
    "864",
    "317",
    "669",
    "861",
    "054",
    "057",
    "671",
    "672",
    "666",
    "2034", 
    "2035",
    "2036",
    "2037",
    "2038",
    "2039",
    "2040",
    "2041",
    "2042",
    "2043",
    "060",      //其它
    ""
};

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的
//完成数据保存功能
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)  
{
    if (strlen((char *)stream) + strlen((char *)ptr) > 999999) return 0;

    strcat((char *)stream, (char *)ptr);
    return size*nmemb;
}

int webGet(char *url, char *str)
{
    CURL *curl;
    str[0] = 0;
    curl=curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);  
    ////CURLOPT_WRITEFUNCTION 将后继的动作交给write_data函数处理
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);//设置超时时间
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);//设置写数据的变量

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return true;
}

int num = 1;
int zhiLianDecode(char *str, struct decodeDate_T *dataInfo)
{
    std::string::const_iterator start, end;  
    string string1 = str;
    start = string1.begin();  
    end = string1.end();  

    boost::regex e2
        ("<td class=\"zwmc\".*? href=\"(.*?)\" target=\"_blank\">(.*?)</a>.*?"   // 职位链接和职位名称
         "<td.*? class=\"fk_lv\".*?<span>(.*?)</span>.*?"                        // 反馈率
         "<td class=\"gsmc\".*? href=\"(.*?)\" target=\"_blank\">(.*?)</a>.*?"   // 公司链接和公司名称  
         "<td class=\"zwyx\">(.*?)</td>.*?"                                      // 月薪
         "<td class=\"gzdd\">(.*?)</td>.*?"                                      // 地点  
         "<td class=\"gxsj\".*?<span>(.*?)</span>.*?");

    boost::regex ptnIndexItem ("_blank\">(.*?)<b>(.*?)</b>(.*?)[&,<]+");
    boost::regex ptnCompany   ("<td class=\"gsmc\">.*?target=\"_blank\">.*?</a>");
    boost::regex ptnSalary    ("<td class=\"zwyx\">.*?</td>");
    boost::regex ptnPlace     ("<td class=\"gzdd\">.*?</td>");


    boost::match_results<std::string::const_iterator> what;  
    boost::match_flag_type flags = boost::match_default;  

    while(regex_search(start, end, what, e2, flags))   
    {     
        boost::match_results<std::string::const_iterator> what_tmp;  
        boost::match_flag_type flags_tmp = boost::match_default;  
        start = what[0].second;  
        //cout << "finded number: " << string(what[0].first,what[0].second) << endl << endl << endl;  

        cout << num++ << ":";
        //获取职位
        if(regex_search(what[0].first, what[0].second, what_tmp, ptnIndexItem, flags_tmp))
            {
                string str_tmp = string(what_tmp[0].first + strlen("_blank\">"), what_tmp[0].second - 1);
                int pos = str_tmp.find("<b>");  
                if (pos >-1)  
                    str_tmp.erase(pos,3);  
                int pos2 = str_tmp.find("</b>");  
                if (pos2 >-1)  
                    str_tmp.erase(pos2,4);  

                strncpy(dataInfo->position, str_tmp.c_str(), sizeof(dataInfo->position));
                //strncpy(dataInfo->position, string(what_tmp[0].first + strlen("_blank\">"), what_tmp[0].second - 1).c_str(), sizeof(dataInfo->position));
                cout << dataInfo->position << "        ";
                //cout  << "        ";
            }

        //获取公司
        if(regex_search(what[0].first, what[0].second, what_tmp, ptnCompany, flags_tmp))
            if(regex_search(what_tmp[0].first, what_tmp[0].second, what_tmp,  boost::regex("target=\"_blank\">.*?</a>"), flags_tmp))
            {
                strncpy(dataInfo->company, string(what_tmp[0].first + strlen("target=\"_blank\">"), what_tmp[0].second - strlen("</a>")).c_str(), sizeof(dataInfo->company));
                cout << dataInfo->company << "        ";
            }


        //获取薪水
        if(regex_search(what[0].first, what[0].second, what_tmp, ptnSalary, flags_tmp))
        {
            cout << string(what_tmp[0].first + strlen("<td class=\"zwyx\">"), what_tmp[0].second - strlen("</td>"));
            cout << "        ";
        }

        //获取薪水
        if(regex_search(what[0].first, what[0].second, what_tmp, ptnPlace, flags_tmp))
        {
            cout << string(what_tmp[0].first + strlen("<td class=\"gzdd\">"), what_tmp[0].second - strlen("</td>"));
        }

        cout << endl;
    }  
    return 0;
}

void zhiLianTask(char *keyword)
{
    char str[1000000];
    char url[1024];
    struct decodeDate_T dataInfo;
    for(int i = 1; i <=90; i++)
    {
        //160000:软件/互联网开发/系统集成
        //https://sou.zhaopin.com/jobs/searchresult.ashx?p=2&bj=160000
        //160000:软件/互联网开发/系统集成 :嵌入式软件开发
        //https://sou.zhaopin.com/jobs/searchresult.ashx?bj=160000&sj=687&p=1
        //160000:软件/互联网开发/系统集成 :嵌入式软件开发&移动互联网开发
        //https://sou.zhaopin.com/jobs/searchresult.ashx?bj=160000&sj=687%3B863&p=1
        //160000:软件/互联网开发/系统集成 :全部
        //https://sou.zhaopin.com/jobs/searchresult.ashx?bj=160000&sj=044%3B045%3B079%3B665%3B667%3B668%3B047%3B048%3B053%3B679%3B687%3B863%3B864%3B317%3B669%3B861%3B054%3B057%3B671%3B672%3B666%3B2034%3B2035%3B2036%3B2037%3B2038%3B2039%3B2040%3B2041%3B2042%3B2043&p=1
        //160000:软件/互联网开发/系统集成 :其它
        https://sou.zhaopin.com/jobs/searchresult.ashx?bj=160000&sj=060&p=1

        //sprintf(url, "https://sou.zhaopin.com/jobs/searchresult.ashx?jl=北京&kw=%s&sm=0&p=%d", keyword, i);
        sprintf(url, "https://sou.zhaopin.com/jobs/searchresult.ashx?jl=北京&kw=%s&sm=0&p=%d", keyword, i);
        cout << url << endl;
        webGet(url, str);
        zhiLianDecode(str, &dataInfo);
        sleep(1);
    }

    return;
}

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    printf("开始收集[%s]相关职位:\r\n", argv[1]);
    zhiLianTask(argv[1]);
    exit(0);
}

