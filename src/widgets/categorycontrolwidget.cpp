#include <QGroupBox>
#include <QVBoxLayout>
#include <QAction>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>

#include "categorycontrolwidget.h"
#include "sliceeditorwidget.h"
#include "renderwidget.h"
#include "model/mrcdatamodel.h"

#include "slicewidget.h"			//enum SliceType
#include "markcategorydialog.h"
#include "model/categorytreeitem.h"
#include <QColorDialog>
#include <QTimer>


/**
 * \brief Constructs a widget by a given \a canvas of \SliceEditorWidget
 */
CategoryControlWidget::CategoryControlWidget(SliceEditorWidget * sliceView, QWidget* parent):
m_canvas(nullptr)

{
	createWidgets();
	setImageCanvas(sliceView);
	connections();
}

/**
 * \brief  Sets the given \a sliceView for the widget to present.
 */

void CategoryControlWidget::setImageCanvas(SliceEditorWidget* sliceView)
{
	if (sliceView == m_canvas)
		return;
	// disconnect signals
	if(m_canvas != nullptr) 
	{
		disconnect(m_canvas, 0, this, 0);
	}
	m_canvas = sliceView;
	connect(m_canvas, &SliceEditorWidget::dataModelChanged, this, &CategoryControlWidget::updateDataModel);
	updateDataModel();
}

/**
 * \brief Returns the current category name for present
 */
QString CategoryControlWidget::currentCategoryName() const
{
	return m_categoryCBBox->currentText();
}

/**
 * \brief  Returns the color of current color
 */
QColor CategoryControlWidget::currentCategoryColor() const
{
	return m_categoryCBBox->currentData().value<QColor>();
}

/**
 * \brief  Returns the number of category
 */
int CategoryControlWidget::categoryCount() const
{
	return m_categoryCBBox->count();
}

void CategoryControlWidget::createWidgets()
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	


	//Mark Group
	auto group = new QGroupBox(QStringLiteral("Mark"), this);
	auto vLayout = new QVBoxLayout;

	//Categroy
	m_categoryLabel = new QLabel(QStringLiteral("Category:"), this);
	m_categoryCBBox = new QComboBox(this);
	m_addCategoryAction = new QToolButton(this);
	m_addCategoryAction->setToolTip(QStringLiteral("Add Category"));
	m_addCategoryAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_addCategoryAction->setIcon(QIcon(":icons/resources/icons/add.png"));

	auto hLayout = new QHBoxLayout;
	hLayout->addWidget(m_categoryLabel);
	hLayout->addWidget(m_categoryCBBox);
	hLayout->addWidget(m_addCategoryAction);
	vLayout->addLayout(hLayout);
	//pen size and color
	m_penSizeLabel = new QLabel(QStringLiteral("PenSize:"), this);
	m_penSizeCBBox = new QComboBox(this);
	for (int i = 1; i <= 30; i++)
		m_penSizeCBBox->addItem(QString::number(i), QVariant::fromValue(i));
	m_colorAction = new QToolButton(this);
	m_colorAction->setToolTip(QStringLiteral("Select Color"));
	m_colorAction->setStyleSheet("QToolButton::menu-indicator{image: none;}");
	m_colorAction->setIcon(QIcon(":icons/resources/icons/color.png"));
	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_penSizeLabel);
	hLayout->addWidget(m_penSizeCBBox);
	hLayout->addWidget(m_colorAction);
	vLayout->addLayout(hLayout);

	vLayout->addLayout(hLayout);
	group->setLayout(vLayout);
	mainLayout->addWidget(group);
	mainLayout->addStretch();

	setLayout(mainLayout);

}


void CategoryControlWidget::updateDataModel()
{
	if (m_canvas == nullptr)
		return;
	const auto m = m_canvas->sliceModel();
	if (m == nullptr) {
		setEnabled(false);
		return;
	}
	setEnabled(true);

	//	Add category
	const auto cates = m_canvas->categories();
	m_categoryCBBox->clear();
	foreach(const auto & c,cates) {
		m_categoryCBBox->addItem(c);
	}
	if(m_categoryCBBox->count() == 0) {
		addCategoryInfoPrivate(QStringLiteral("Default"), Qt::red);
		QPen pen = m_canvas->topView()->pen();
		pen.setColor(Qt::red);
		m_canvas->setPen(pen);
	}
}


void CategoryControlWidget::connections()
{
	connect(m_addCategoryAction, &QToolButton::clicked, this, &CategoryControlWidget::onCategoryAdded);
	connect(m_penSizeCBBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int) {QPen pen = m_canvas->topView()->pen(); pen.setWidth(m_penSizeCBBox->currentData().toInt()); m_canvas->setPen(pen); });
	connect(m_colorAction, &QToolButton::clicked, this, &CategoryControlWidget::colorChanged);
	connect(m_categoryCBBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this](const QString & text) {
		QPen pen = m_canvas->topView()->pen();
		pen.setColor(m_categoryCBBox->currentData().value<QColor>());
		m_canvas->setPen(pen);
		m_canvas->setCurrentCategory(text);
	});
}

void CategoryControlWidget::setCategoryInfoPrivate(const QVector<QPair<QString, QColor>>& cates)
{
	Q_ASSERT_X(m_categoryCBBox, "ImageVIew::initCCBoxHelper", "null pointer");
	m_categoryCBBox->clear();
	foreach(const auto & p, cates)
		m_categoryCBBox->addItem(p.first, QVariant::fromValue(p.second));

}

void CategoryControlWidget::addCategoryInfoPrivate(const QString & name, const QColor & color)
{
	m_categoryCBBox->addItem(name, color);
	m_categoryCBBox->setCurrentText(name);
	//auto ci = CategoryInfo(name, color);
	m_canvas->addCategory(CategoryInfo(name,color));
	m_canvas->setCurrentCategory(name);
}


QIcon CategoryControlWidget::createColorIcon(const QColor& color)
{
	constexpr int iconSize = 64;
	constexpr int barHeight = 20;

	QPixmap pixmap(iconSize, iconSize + barHeight);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QPixmap image(":icons/resources/icons/color.png");
	// Draw icon centred horizontally on button.
	QRect target(0, 0, iconSize, iconSize);
	painter.fillRect(QRect(0, iconSize, iconSize, barHeight), color);
	painter.drawPixmap(target, image);
	return QIcon(pixmap);
}

void CategoryControlWidget::onCategoryAdded()
{
	MarkCategoryDialog dlg(this);
	connect(&dlg, &MarkCategoryDialog::resultReceived, [this](const QString & name, const QColor & color)
	{
		addCategoryInfoPrivate(name, color);
		QPen pen = m_canvas->topView()->pen();
		pen.setColor(color);
		m_canvas->setPen(pen);
		///TODO:: This color need to be add categoryItem

	});
	dlg.exec();
}

void CategoryControlWidget::colorChanged()
{
	Q_ASSERT_X(m_canvas, "ImageViewControlPanel::colorChanged", "null pointer");

	auto d = m_categoryCBBox->itemData(m_categoryCBBox->currentIndex());
	QColor defaultColor = d.canConvert<QColor>() ? d.value<QColor>() : Qt::black;
	QPen pen = m_canvas->pen();
	pen.setColor(QColorDialog::getColor(defaultColor, this, QStringLiteral("Color")));
	m_canvas->setPen(pen);
}

