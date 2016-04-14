//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//boost�����ĵ���ʵʱ�������Ŀͻ���
//ʵ�ֵĴ���˼·�ǣ��ڴ����Ŀͻ���ʵ���г�ʼ��socket�����ӷ������ˣ����Ҳ��ϵĽ������첽���������ӷ������˶����ݣ�
//�����߳��У���console�в��϶�ȡҪ�����͵���Ϣ�����Ұ���Щ��Ϣpost��io_service��Ȼ������첽д����
//��дsocket�������첽����
//���ַ�����ͬ�ڷֱ�һ�����̣߳�һ��д�̣߳� �����������̲߳���һֱ�ȴ���д���ݣ��ڲ�������������ͨ���첽��д�������Դ������


#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service),
      socket_(io_service) //ʹ�ó�Ա������ֱ��ʹ����Щ����
  {
    boost::asio::async_connect(socket_, endpoint_iterator,
        boost::bind(&chat_client::handle_connect, this,
          boost::asio::placeholders::error)); //���еĲ����������첽�ķ�ʽ
  }

  void write(const chat_message& msg)
  {
    io_service_.post(boost::bind(&chat_client::do_write, this, msg)); //����Ϣ����Ͷ�ݸ�io_service
  }

  void close()
  {
    io_service_.post(boost::bind(&chat_client::do_close, this)); //���close�����ǿͻ���Ҫ������ֹʱ����  do_close�����Ǵӷ�������
                                                                //������ʧ��ʱ����
  }

private:

  void handle_connect(const boost::system::error_code& error)
  {
    if (!error)
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), chat_message::header_length), //��ȡ���ݱ�ͷ
          boost::bind(&chat_client::handle_read_header, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if (!error && read_msg_.decode_header()) //�ֱ������ݱ�ͷ�����ݲ���
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),//��ȡ���ݰ����ݲ���
          boost::bind(&chat_client::handle_read_body, this,
            boost::asio::placeholders::error));
    }
    else
    {
      do_close();
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::cout.write(read_msg_.body(), read_msg_.body_length()); //�����Ϣ
      std::cout << "\n";
      boost::asio::async_read(socket_, 
          boost::asio::buffer(read_msg_.data(), chat_message::header_length), //�������ȡ��һ�����ݰ�ͷ
          boost::bind(&chat_client::handle_read_header, this,  
            boost::asio::placeholders::error)); //���һ�ζ�������������һ�����ݰ���  ������һ�ζ�����
    }
    else
    {
      do_close();
    }
  }

  void do_write(chat_message msg)
  {
    bool write_in_progress = !write_msgs_.empty(); //�յĻ�����Ϊfalse
    write_msgs_.push_back(msg); //��Ҫд������push��д����
    if (!write_in_progress)//���г�ʼΪ�� pushһ��msg�����һ��Ԫ����
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          boost::bind(&chat_client::handle_write, this, 
            boost::asio::placeholders::error));
    }
  }

  void handle_write(const boost::system::error_code& error)//��һ����Ϣ��������ʣ�µĲŸ��ò���
  {
    if (!error)
    {
      write_msgs_.pop_front();//�ղŴ�����һ������ ����Ҫpopһ��
      if (!write_msgs_.empty())  
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            boost::bind(&chat_client::handle_write, this,
              boost::asio::placeholders::error)); //ѭ������ʣ�����Ϣ
      }
    }
    else
    {
      do_close();
    }
  }

  void do_close()
  {
    socket_.close();
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]); //����ip�����������Ͷ˿ں�
    tcp::resolver::iterator iterator = resolver.resolve(query);

    chat_client c(io_service, iterator);

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      using namespace std; // For strlen and memcpy.
      chat_message msg;
      msg.body_length(strlen(line));
      memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}