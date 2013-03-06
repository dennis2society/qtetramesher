/********************************************************************************
** Form generated from reading UI file 'tetrastuffingdialog.ui'
**
** Created: Wed Mar 6 21:18:29 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TETRASTUFFINGDIALOG_H
#define UI_TETRASTUFFINGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TetraStuffingDialog
{
public:
    QDialogButtonBox *buttonBox;
    QFrame *frame;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *tetraSizeLabel;
    QDoubleSpinBox *tetraSizeSpinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *alphaShortLabel;
    QDoubleSpinBox *alphaShortSpinBox;
    QLabel *alphaLongLabel;
    QDoubleSpinBox *alphaLongSpinBox;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *splitTetrasCheckBox;
    QCheckBox *snapToPointsCheckBox;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *generateTetrasButton;

    void setupUi(QDialog *TetraStuffingDialog)
    {
        if (TetraStuffingDialog->objectName().isEmpty())
            TetraStuffingDialog->setObjectName(QString::fromUtf8("TetraStuffingDialog"));
        TetraStuffingDialog->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TetraStuffingDialog->sizePolicy().hasHeightForWidth());
        TetraStuffingDialog->setSizePolicy(sizePolicy);
        TetraStuffingDialog->setMinimumSize(QSize(400, 300));
        TetraStuffingDialog->setMaximumSize(QSize(400, 300));
        buttonBox = new QDialogButtonBox(TetraStuffingDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        frame = new QFrame(TetraStuffingDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 401, 251));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayoutWidget = new QWidget(frame);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 401, 251));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(2, 0, 2, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(6, -1, 6, -1);
        tetraSizeLabel = new QLabel(verticalLayoutWidget);
        tetraSizeLabel->setObjectName(QString::fromUtf8("tetraSizeLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        tetraSizeLabel->setFont(font);

        horizontalLayout->addWidget(tetraSizeLabel);

        tetraSizeSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        tetraSizeSpinBox->setObjectName(QString::fromUtf8("tetraSizeSpinBox"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        tetraSizeSpinBox->setFont(font1);
        tetraSizeSpinBox->setSingleStep(0.1);
        tetraSizeSpinBox->setValue(2);

        horizontalLayout->addWidget(tetraSizeSpinBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(6, -1, 6, -1);
        alphaShortLabel = new QLabel(verticalLayoutWidget);
        alphaShortLabel->setObjectName(QString::fromUtf8("alphaShortLabel"));
        alphaShortLabel->setFont(font);

        horizontalLayout_2->addWidget(alphaShortLabel);

        alphaShortSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        alphaShortSpinBox->setObjectName(QString::fromUtf8("alphaShortSpinBox"));
        alphaShortSpinBox->setFont(font1);
        alphaShortSpinBox->setMaximum(1);
        alphaShortSpinBox->setSingleStep(0.01);
        alphaShortSpinBox->setValue(0.43);

        horizontalLayout_2->addWidget(alphaShortSpinBox);

        alphaLongLabel = new QLabel(verticalLayoutWidget);
        alphaLongLabel->setObjectName(QString::fromUtf8("alphaLongLabel"));
        alphaLongLabel->setFont(font);

        horizontalLayout_2->addWidget(alphaLongLabel);

        alphaLongSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        alphaLongSpinBox->setObjectName(QString::fromUtf8("alphaLongSpinBox"));
        alphaLongSpinBox->setFont(font1);
        alphaLongSpinBox->setMaximum(1);
        alphaLongSpinBox->setSingleStep(0.01);
        alphaLongSpinBox->setValue(0.25);

        horizontalLayout_2->addWidget(alphaLongSpinBox);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(6, -1, 6, -1);
        splitTetrasCheckBox = new QCheckBox(verticalLayoutWidget);
        splitTetrasCheckBox->setObjectName(QString::fromUtf8("splitTetrasCheckBox"));
        splitTetrasCheckBox->setFont(font);
        splitTetrasCheckBox->setChecked(true);

        horizontalLayout_3->addWidget(splitTetrasCheckBox);

        snapToPointsCheckBox = new QCheckBox(verticalLayoutWidget);
        snapToPointsCheckBox->setObjectName(QString::fromUtf8("snapToPointsCheckBox"));
        snapToPointsCheckBox->setFont(font);
        snapToPointsCheckBox->setChecked(true);

        horizontalLayout_3->addWidget(snapToPointsCheckBox);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(6, 0, 6, 0);
        generateTetrasButton = new QPushButton(verticalLayoutWidget);
        generateTetrasButton->setObjectName(QString::fromUtf8("generateTetrasButton"));
        generateTetrasButton->setFont(font);

        horizontalLayout_4->addWidget(generateTetrasButton);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(TetraStuffingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), TetraStuffingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TetraStuffingDialog, SLOT(reject()));
        QObject::connect(generateTetrasButton, SIGNAL(released()), TetraStuffingDialog, SLOT(generateTetras()));

        QMetaObject::connectSlotsByName(TetraStuffingDialog);
    } // setupUi

    void retranslateUi(QDialog *TetraStuffingDialog)
    {
        TetraStuffingDialog->setWindowTitle(QApplication::translate("TetraStuffingDialog", "TetraStuffing Options", 0, QApplication::UnicodeUTF8));
        tetraSizeLabel->setText(QApplication::translate("TetraStuffingDialog", "TetraSize", 0, QApplication::UnicodeUTF8));
        alphaShortLabel->setText(QApplication::translate("TetraStuffingDialog", "AlphaShort", 0, QApplication::UnicodeUTF8));
        alphaLongLabel->setText(QApplication::translate("TetraStuffingDialog", "AlphaLong", 0, QApplication::UnicodeUTF8));
        splitTetrasCheckBox->setText(QApplication::translate("TetraStuffingDialog", "Split Tetras", 0, QApplication::UnicodeUTF8));
        snapToPointsCheckBox->setText(QApplication::translate("TetraStuffingDialog", "Snap To Points", 0, QApplication::UnicodeUTF8));
        generateTetrasButton->setText(QApplication::translate("TetraStuffingDialog", "Generate Tetrahedra", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TetraStuffingDialog: public Ui_TetraStuffingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TETRASTUFFINGDIALOG_H
