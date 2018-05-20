/********************************************************************************
** Form generated from reading UI file 'CGALTetrahedralizeDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CGALTETRAHEDRALIZEDIALOG_H
#define UI_CGALTETRAHEDRALIZEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CGALTetrahedralizeDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QLabel *cellSizeLabel;
    QDoubleSpinBox *cellSizeSpinBox;
    QHBoxLayout *horizontalLayout_5;
    QLabel *facetAngleLabel;
    QDoubleSpinBox *facetAngleSpinBox;
    QHBoxLayout *horizontalLayout_3;
    QLabel *facetSizeLabel;
    QDoubleSpinBox *facetSizeSpinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *facetDistanceLabel;
    QDoubleSpinBox *facetDistanceSpinBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *cellRadiusEdgeRatioLabel;
    QDoubleSpinBox *crterSpinBox;
    QPushButton *CGALTetrahedralizeButton;

    void setupUi(QDialog *CGALTetrahedralizeDialog)
    {
        if (CGALTetrahedralizeDialog->objectName().isEmpty())
            CGALTetrahedralizeDialog->setObjectName(QStringLiteral("CGALTetrahedralizeDialog"));
        CGALTetrahedralizeDialog->resize(404, 332);
        buttonBox = new QDialogButtonBox(CGALTetrahedralizeDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(60, 300, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        verticalLayoutWidget = new QWidget(CGALTetrahedralizeDialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 401, 251));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        cellSizeLabel = new QLabel(verticalLayoutWidget);
        cellSizeLabel->setObjectName(QStringLiteral("cellSizeLabel"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        cellSizeLabel->setFont(font);

        horizontalLayout_6->addWidget(cellSizeLabel);

        cellSizeSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        cellSizeSpinBox->setObjectName(QStringLiteral("cellSizeSpinBox"));
        cellSizeSpinBox->setFont(font);
        cellSizeSpinBox->setMinimum(0.01);
        cellSizeSpinBox->setMaximum(50);
        cellSizeSpinBox->setSingleStep(0.05);
        cellSizeSpinBox->setValue(0.5);

        horizontalLayout_6->addWidget(cellSizeSpinBox);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        facetAngleLabel = new QLabel(verticalLayoutWidget);
        facetAngleLabel->setObjectName(QStringLiteral("facetAngleLabel"));
        facetAngleLabel->setFont(font);

        horizontalLayout_5->addWidget(facetAngleLabel);

        facetAngleSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        facetAngleSpinBox->setObjectName(QStringLiteral("facetAngleSpinBox"));
        facetAngleSpinBox->setFont(font);
        facetAngleSpinBox->setDecimals(1);
        facetAngleSpinBox->setMinimum(1);
        facetAngleSpinBox->setMaximum(60);
        facetAngleSpinBox->setValue(25);

        horizontalLayout_5->addWidget(facetAngleSpinBox);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        facetSizeLabel = new QLabel(verticalLayoutWidget);
        facetSizeLabel->setObjectName(QStringLiteral("facetSizeLabel"));
        facetSizeLabel->setFont(font);

        horizontalLayout_3->addWidget(facetSizeLabel);

        facetSizeSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        facetSizeSpinBox->setObjectName(QStringLiteral("facetSizeSpinBox"));
        facetSizeSpinBox->setFont(font);
        facetSizeSpinBox->setMinimum(0.01);
        facetSizeSpinBox->setSingleStep(0.05);
        facetSizeSpinBox->setValue(0.5);

        horizontalLayout_3->addWidget(facetSizeSpinBox);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        facetDistanceLabel = new QLabel(verticalLayoutWidget);
        facetDistanceLabel->setObjectName(QStringLiteral("facetDistanceLabel"));
        facetDistanceLabel->setFont(font);

        horizontalLayout_2->addWidget(facetDistanceLabel);

        facetDistanceSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        facetDistanceSpinBox->setObjectName(QStringLiteral("facetDistanceSpinBox"));
        facetDistanceSpinBox->setFont(font);
        facetDistanceSpinBox->setMinimum(0.01);
        facetDistanceSpinBox->setMaximum(10);
        facetDistanceSpinBox->setSingleStep(0.01);
        facetDistanceSpinBox->setValue(0.03);

        horizontalLayout_2->addWidget(facetDistanceSpinBox);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        cellRadiusEdgeRatioLabel = new QLabel(verticalLayoutWidget);
        cellRadiusEdgeRatioLabel->setObjectName(QStringLiteral("cellRadiusEdgeRatioLabel"));
        cellRadiusEdgeRatioLabel->setFont(font);

        horizontalLayout_4->addWidget(cellRadiusEdgeRatioLabel);

        crterSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        crterSpinBox->setObjectName(QStringLiteral("crterSpinBox"));
        crterSpinBox->setFont(font);
        crterSpinBox->setDecimals(1);
        crterSpinBox->setMinimum(0.1);
        crterSpinBox->setMaximum(10);
        crterSpinBox->setSingleStep(0.1);
        crterSpinBox->setValue(3);

        horizontalLayout_4->addWidget(crterSpinBox);


        verticalLayout->addLayout(horizontalLayout_4);

        CGALTetrahedralizeButton = new QPushButton(CGALTetrahedralizeDialog);
        CGALTetrahedralizeButton->setObjectName(QStringLiteral("CGALTetrahedralizeButton"));
        CGALTetrahedralizeButton->setGeometry(QRect(27, 260, 351, 27));
        CGALTetrahedralizeButton->setFont(font);

        retranslateUi(CGALTetrahedralizeDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CGALTetrahedralizeDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CGALTetrahedralizeDialog, SLOT(reject()));
        QObject::connect(CGALTetrahedralizeButton, SIGNAL(released()), CGALTetrahedralizeDialog, SLOT(generateTetras()));

        QMetaObject::connectSlotsByName(CGALTetrahedralizeDialog);
    } // setupUi

    void retranslateUi(QDialog *CGALTetrahedralizeDialog)
    {
        CGALTetrahedralizeDialog->setWindowTitle(QApplication::translate("CGALTetrahedralizeDialog", "CGAL Tetrahedralize Options", Q_NULLPTR));
        cellSizeLabel->setText(QApplication::translate("CGALTetrahedralizeDialog", "Cell Size", Q_NULLPTR));
        facetAngleLabel->setText(QApplication::translate("CGALTetrahedralizeDialog", "Facet Angle", Q_NULLPTR));
        facetSizeLabel->setText(QApplication::translate("CGALTetrahedralizeDialog", "Facet Size", Q_NULLPTR));
        facetDistanceLabel->setText(QApplication::translate("CGALTetrahedralizeDialog", "Facet Distance", Q_NULLPTR));
        cellRadiusEdgeRatioLabel->setText(QApplication::translate("CGALTetrahedralizeDialog", "CellRadius To Edge Ratio", Q_NULLPTR));
        CGALTetrahedralizeButton->setText(QApplication::translate("CGALTetrahedralizeDialog", "Generate Tetrahedra", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CGALTetrahedralizeDialog: public Ui_CGALTetrahedralizeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CGALTETRAHEDRALIZEDIALOG_H
