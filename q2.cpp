#include <QAxObject>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QColor>
#include "qexcel.h"

QExcel::QExcel()
{
}

QExcel::QExcel(QString xlsFilePath, QObject *parent)
{
	excel = 0;
	workBooks = 0;
	workBook = 0;
	sheets = 0;
	sheet = 0;
	m_xlsFilePath = xlsFilePath;
	excel = new QAxObject("Excel.Application", parent);
	workBooks = excel->querySubObject("Workbooks");
	workBooks->dynamicCall("Add");//新建一个工作簿
	excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体
	excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
	QFile file(xlsFilePath);
	//if (file.exists())
	//{
	if(!xlsFilePath.isEmpty())
	{
		//workBooks->dynamicCall("Open(const QString&)", xlsFilePath);
		//workBooks->dynamicCall("Open(const QString&)", xlsFilePath);
		workBook = excel->querySubObject("ActiveWorkBook");
		sheets = workBook->querySubObject("Sheets");
	}
	
}

QExcel::~QExcel()
{
	close();
}

void QExcel::close()
{
	if (excel == NULL)
	{
		return;
		
	}
	excel->dynamicCall("Quit()");
	delete sheet;
	delete sheets;
	delete workBook;
	delete workBooks;
	delete excel;

	excel = 0;
	workBooks = 0;
	workBook = 0;
	sheets = 0;
	sheet = 0;
}


QAxObject *QExcel::getWorkBooks()
{
	return workBooks;
}

QAxObject *QExcel::getWorkBook()
{
	return workBook;
}

QAxObject *QExcel::getWorkSheets()
{
	return sheets;
}

QAxObject *QExcel::getWorkSheet()
{
	return sheet;
}

void QExcel::ShowExcel(bool Visible)
{

	excel->setProperty("Visible",Visible);
}

void QExcel::selectSheet(int sheetIndex, const QString& sheetName)
{
	
	//sheet = sheets->querySubObject("Item(int)", 1);
	QAxObject * a = sheets->querySubObject("Item(int)", sheetIndex);
	//a->dynamicCall("delete");
	a->setProperty("Name", sheetName);
}

void QExcel::SetLockedSheet(const QString& password)
{
	sheet->dynamicCall("Protect(Password)", password);
}

void QExcel::deleteSheet(const QString& sheetName)
{
	QAxObject * a = sheets->querySubObject("Item(const QString&)", sheetName);
	a->dynamicCall("delete");
}

void QExcel::deleteSheet(int sheetIndex)
{
	QAxObject * a = sheets->querySubObject("Item(int)", sheetIndex);
	a->dynamicCall("delete");
}

void QExcel::selectSheet(int sheetIndex)
{
	sheet = sheets->querySubObject("Item(int)", sheetIndex);
}

void QExcel::setCellString(int row, int column, QString value)
{
	QVariant var(value);
	QAxObject *range = sheet->querySubObject("Cells(int,int)", row, column);
	range->dynamicCall("SetValue(const QString&)", var);
}

void QExcel::setMoreCellString(const QString& cell, QList<QList<QVariant>>datas)
{
	//二维数组转一维
	QList<QVariant> vars;

	for (auto v : datas)
	{
		vars.append(QVariant(v));
	}
	//一维数组转变量
	QVariant var = QVariant(vars);

	QAxObject *user_range = sheet->querySubObject("Range(const QString&)", cell);//指定范围
	user_range->setProperty("Value", var);//调用一次QAxObject即可完成写入
	user_range->setProperty("HorizontalAlignment", -4108);
}

void QExcel::setCellFontBold(int row, int column, bool isBold)
{
	QString cell;
	cell.append(QChar(column - 1 + 'A'));
	cell.append(QString::number(row));

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range = range->querySubObject("Font");
	range->setProperty("Bold", isBold);
}

void QExcel::setCellFont(const QString& cell, QString font)
{
	QVariant var(font);
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range = range->querySubObject("Font");
	range->setProperty("Name", var);
}

void QExcel::setCellFontSize(int row, int column, int size)
{
	QString cell;
	cell.append(QChar(column - 1 + 'A'));
	cell.append(QString::number(row));

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range = range->querySubObject("Font");
	range->setProperty("Size", size);
}

void QExcel::setRowBorders(const QString& cell, QColor color)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	QAxObject* interior = range->querySubObject("Borders");
	interior->setProperty("Color", color);
}

void QExcel::mergeCells(const QString& cell)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("VerticalAlignment", -4108);//xlCenter
	range->setProperty("WrapText", true);
	range->setProperty("MergeCells", true);
}

void QExcel::mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn)
{
	QString cell;
	cell.append(QChar(topLeftColumn - 1 + 'A'));
	cell.append(QString::number(topLeftRow));
	cell.append(":");
	cell.append(QChar(bottomRightColumn - 1 + 'A'));
	cell.append(QString::number(bottomRightRow));

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("VerticalAlignment", -4108);//xlCenter
	range->setProperty("WrapText", true);
	range->setProperty("MergeCells", true);
}

QVariant QExcel::getCellValue(int row, int column)
{
	QAxObject *range = sheet->querySubObject("Cells(int,int)", row, column);
	return range->property("Value");
}

void QExcel::save()
{
	workBook->dynamicCall("Save()");
	
	
}

void QExcel::saveFile()
{
	workBook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(m_xlsFilePath));
}

int QExcel::getSheetsCount()
{
	return sheets->property("Count").toInt();
}

QString QExcel::getSheetName()
{
	return sheet->property("Name").toString();
}

QString QExcel::getSheetName(int sheetIndex)
{
	QAxObject * a = sheets->querySubObject("Item(int)", sheetIndex);
	return a->property("Name").toString();
}

//设置背景颜色  橘黄色QColor(255, 192, 0)
void QExcel::setBackGroundColor(const QString& cell, QColor color)
{

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	QAxObject* interior = range->querySubObject("Interior");
	interior->setProperty("Color", color);
}

void QExcel::getUsedRange(int *topLeftRow, int *topLeftColumn, int *bottomRightRow, int *bottomRightColumn)
{
	QAxObject *usedRange = sheet->querySubObject("UsedRange");
	*topLeftRow = usedRange->property("Row").toInt();
	*topLeftColumn = usedRange->property("Column").toInt();

	QAxObject *rows = usedRange->querySubObject("Rows");
	*bottomRightRow = *topLeftRow + rows->property("Count").toInt() - 1;

	QAxObject *columns = usedRange->querySubObject("Columns");
	*bottomRightColumn = *topLeftColumn + columns->property("Count").toInt() - 1;
}


void QExcel::setColumnWidth(int column, int width)
{
	QString columnName;
	columnName.append(QChar(column - 1 + 'A'));
	columnName.append(":");
	columnName.append(QChar(column - 1 + 'A'));

	QAxObject * col = sheet->querySubObject("Columns(const QString&)", columnName);
	col->setProperty("ColumnWidth", width);
}

//设置单元格文字居中
void QExcel::setCellTextCenter(int row, int column)
{
	QString cell;
	cell.append(QChar(column - 1 + 'A'));
	cell.append(QString::number(row));

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("HorizontalAlignment", -4108);//xlCenter
}

//设置单元格文字自动折行
void QExcel::setCellTextWrap(int row, int column, bool isWrap)
{
	QString cell;
	cell.append(QChar(column - 1 + 'A'));
	cell.append(QString::number(row));

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("WrapText", isWrap);
}

//设置一行自适应行高
void QExcel::setAutoFitRow(int row)
{
	QString rowsName;
	rowsName.append(QString::number(row));
	rowsName.append(":");
	rowsName.append(QString::number(row));

	QAxObject * rows = sheet->querySubObject("Rows(const QString &)", rowsName);
	rows->dynamicCall("AutoFit()");
}

void QExcel::insertSheet(QString sheetName)
{
	sheets->querySubObject("Add()");
	QAxObject * a = sheets->querySubObject("Item(int)", 1);
	a->setProperty("Name", sheetName);
}

void QExcel::mergeSerialSameCellsInAColumn(int column, int topRow)
{
	int a,b,c,rowsCount;
	getUsedRange(&a, &b, &rowsCount, &c);

	int aMergeStart = topRow, aMergeEnd = topRow + 1;

	QString value;
	while(aMergeEnd <= rowsCount)
	{
		value = getCellValue(aMergeStart, column).toString();
		while(value == getCellValue(aMergeEnd, column).toString())
		{
			clearCell(aMergeEnd, column);
			aMergeEnd++;
		}
		aMergeEnd--;
		mergeCells(aMergeStart, column, aMergeEnd, column);

		aMergeStart = aMergeEnd + 1;
		aMergeEnd = aMergeStart + 1;
	}
}

void QExcel::clearCell(int row, int column)
{
	QString cell;
	cell.append(QChar(column - 1 + 'A'));
	cell.append(QString::number(row));

	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->dynamicCall("ClearContents()");
}

void QExcel::clearCell(const QString& cell)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->dynamicCall("ClearContents()");
}

//获取一张工作表已用行数
int QExcel::getUsedRowsCount()
{
	QAxObject *usedRange = sheet->querySubObject("UsedRange");
	int topRow = usedRange->property("Row").toInt();
	QAxObject *rows = usedRange->querySubObject("Rows");
	int bottomRow = topRow + rows->property("Count").toInt() - 1;
	return bottomRow;
}

void QExcel::setCellString(const QString& cell, const QString& value)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->dynamicCall("SetValue(const QString&)", value);
}

//设置文字大小
void QExcel::setCellFontSize(const QString &cell, int size)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range = range->querySubObject("Font");
	range->setProperty("Size", size);
}

void QExcel::setCellTextCenter(const QString &cell)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("HorizontalAlignment", -4108);//xlCenter
}

void QExcel::setCellFontBold(const QString &cell, bool isBold)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range = range->querySubObject("Font");
	range->setProperty("Bold", isBold);
}

void QExcel::setCellTextWrap(const QString &cell, bool isWrap)
{
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("WrapText", isWrap);
}

void QExcel::setRowHeight(int row, int height)
{
	QString rowsName;
	rowsName.append(QString::number(row));
	rowsName.append(":");
	rowsName.append(QString::number(row));

	QAxObject * r = sheet->querySubObject("Rows(const QString &)", rowsName);
	r->setProperty("RowHeight", height);
}

void QExcel::freezePanes(int row, int column, bool freeze)
{

	QAxObject * Objwindows = excel->querySubObject("ActiveWindow");
	if (row <= 0 && column <= 0)
	{
		freeze = false;
		return;
	}

	Objwindows->setProperty("SplitRow", qMax(0, row));
	Objwindows->setProperty("SplitColumn", qMax(0, column));
	Objwindows->setProperty("FreezePanes", freeze);
	delete Objwindows;
}
