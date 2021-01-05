#pragma once
#include <QBuffer>
#include <QSharedMemory>
#include <QDataStream>

QSharedMemory *qtsharememory_; //定义共享内存实例指针

class qtsharememory
{
public:
	qtsharememory()//构造函数
	{
	}
	~qtsharememory()//析构函数
	{
	}
	static int qtsharedmemoryinit()//共享内存初始化
	{
		if (qtsharememory_==NULL)
		{
			qtsharememory_ = NULL;
			qtsharememory_ = new QSharedMemory();              //构造实例对象
			qtsharememory_->setKey("qtsharedmemory_global");    //为实例对象指定关键字(给共享内存命名)
		}
		return 0;
	}
	static int qtwritesharedmemory(QString inData)//写入共享内存
	{
		if (inData.isEmpty())return -5;//没有数据

		if (qtsharememory_ == NULL)
		{
			qtsharedmemoryinit();//共享内存初始化
		}
		if (qtsharememory_->isAttached()) {
			if (!qtsharememory_->detach()) {
				
				return -1;//共享内存关联失败
			}
		}
		QBuffer buffer;
		buffer.open(QBuffer::ReadWrite);
		QDataStream out(&buffer);
		out << inData;
		int size = buffer.size();

		// 创建共享内存段
		if (!qtsharememory_->create(size))
		{
			//qDebug() << qtsharememory_->errorString() << "\n Unable to create shared memory segment.";
			//创建共享内存失败
			return -2;
		}
		qtsharememory_->lock();
		char *to = (char*)qtsharememory_->data();
		const char *from = buffer.data().data();
		memcpy(to, from, qMin(qtsharememory_->size(), size));
		qtsharememory_->unlock();
		return 0;//写入成功
	}
	static int qtreadsharedmemory(QString& outData)//读取共享内存
	{
		if (qtsharememory_ == NULL)
		{
			qtsharedmemoryinit();//共享内存初始化
		}
		if (!qtsharememory_->isAttached())//判断是否关联
		{
			if (!qtsharememory_->attach())
			{
				//共享内存关联失败
				return -1;
			}
		}

		QBuffer buffer;                //构建缓冲区
		QDataStream out(&buffer);      //建立数据流对象，并和缓冲区关联
		//对共享内存进行读写操作
		qtsharememory_->lock();           //锁定共享内存
									   //初始化缓冲区中的数据，setData函数用来初始化缓冲区。
									   //该函数如果在open()函数之后被调用，则不起任何作用。
									   //buffer.open(QBuffer::ReadOnly);  //解除注释，则setData函数不起作用，无法加载内存中数据
		buffer.setData((char*)qtsharememory_->constData(), qtsharememory_->size());
		buffer.open(QBuffer::ReadOnly);    //只读方式打开缓冲区
		out >> outData;                      //将缓冲区的数据写入QImage对象
		qtsharememory_->unlock();         //释放共享内存
		qtsharememory_->detach();         //解除程序和共享内存的关联----------------解除后，另一个程序才能关联成功，重新写入---最好成对使用
		return 0;//读取成功
	}
};
