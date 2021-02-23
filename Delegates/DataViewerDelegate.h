#pragma once

#include <QStyledItemDelegate>

#include "../DefinedStructs/ConstDataHolder.hpp"

#include <qpainter.h>

class DataViewerDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	DataViewerDelegate(bool dataHighlight, QObject *parent);
	~DataViewerDelegate();
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
	void drawDataHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QVariantList& list) const;
	void drawOtherDataHighlight(QPainter* painter, DataTypeColor& color, QRect& rect) const;
	void drawControlHeaderDataHiglight(QPainter* painter, DataTypeColor& color, QRect& rect) const;

	bool dataHighlight;
	DataHolder* holder;
};
