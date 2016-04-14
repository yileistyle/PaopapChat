<pre name="code" class="cpp"><h3>���ݰ�chat_message.hpp</h3>  
// chat_message.hpp  
// ~~~~~~~~~~~~~~~~  
//  
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)  
//  
// Distributed under the Boost Software License, Version 1.0. (See accompanying  
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)  
//ʵ��Դ��http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp03_examples.html �ĵ���û��ע�ͣ��Լ���///��ʼ����ʱ��Ҳ���˵���·�����ĶԸ�Դ��д����ϸ��ע�ͣ�ϣ�����ú�����������Щ��·��  
//���ȿ������ݰ��Ľṹ��  
//���ݰ���Ϊ�������֣������Ǳ�ͷռ4bytes��ÿһbyte��ʾһ�����֣�Ҳ��������ܱ�ʾ�ĳ�9999��������ǰ��0��  
// ���ݰ�ͷ�����ֱ�ʾ���ݲ��ֵĳ��ȴ�С��   
  
#ifndef CHAT_MESSAGE_HPP  
#define CHAT_MESSAGE_HPP  
  
#include <cstdio>  
#include <cstdlib>  
#include <cstring>  
  
class chat_message  
{  
public:  
  enum { header_length = 4 };  
  enum { max_body_length = 512 };  
  
  chat_message()  
    : body_length_(0)  
  {  
  }  
  
  const char* data() const  
  {  
    return data_;  
  }  
  
  char* data()  
  {  
    return data_;  
  }  
  
  size_t length() const  
  {  
    return header_length + body_length_;  
  }  
  
  const char* body() const  
  {  
    return data_ + header_length;  
  }  
  
  char* body()  
  {  
    return data_ + header_length;  
  }  
  
  size_t body_length() const  
  {  
    return body_length_;  
  }  
  
  void body_length(size_t new_length)  
  {  
    body_length_ = new_length;  
    if (body_length_ > max_body_length)  
      body_length_ = max_body_length;  
  }  
  
  bool decode_header()//����ͷ��4�ֽ��ַ���ת��������  
  {  
    using namespace std; // For strncat and atoi.  
    char header[header_length + 1] = "";  
    strncat(header, data_, header_length);  
    body_length_ = atoi(header);  
    if (body_length_ > max_body_length)  
    {  
      body_length_ = 0;  
      return false;  
    }  
    return true;  
  }  
  
  void encode_header()//�����ݲ��ִ�С������ַ���  
  {  
    using namespace std; // For sprintf and memcpy.  
    char header[header_length + 1] = "";  
    sprintf(header, "%4d", body_length_);  
    memcpy(data_, header, header_length);  
  }  
  
private:  
  char data_[header_length + max_body_length];  
  size_t body_length_;  
};  