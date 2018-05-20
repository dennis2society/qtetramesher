/********************************************************************************
** Form generated from reading UI file 'tetrastuffingdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TETRASTUFFINGDIALOG_H
#define UI_TETRASTUFFINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

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
            TetraStuffingDialog->setObjectName(QStringLiteral("TetraStuffingDialog"));
        TetraStuffingDialog->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TetraStuffingDialog->sizePolicy().hasHeightForWidth());
        TetraStuffingDialog->setSizePolicy(sizePolicy);
        TetraStuffingDialog->setMinimumSize(QSize(400, 300));
        TetraStuffingDialog->setMaximumSize(QSize(400, 300));
        buttonBox = new QDialogButtonBox(TetraStuffingDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        frame = new QFrame(TetraStuffingDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 401, 251));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayoutWidget = new QWidget(frame);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 401, 251));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 0, 2, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(6, -1, 6, -1);
        tetraSizeLabel = new QLabel(verticalLayoutWidget);
        tetraSizeLabel->setObjectName(QStringLiteral("tetraSizeLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        tetraSizeLabel->setFont(font);

        horizontalLayout->addWidget(tetraSizeLabel);

        tetraSizeSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        tetraSizeSpinBox->setObjectName(QStringLiteral("tetraSizeSpinBox"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        tetraSizeSpinBox->setFont(font1);
        tetraSizeSpinBox->setSingleStep(0.1);
        tetraSizeSpinBox->setValue(2);

        horizontalLayout->addWidget(tetraSizeSpinBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(6, -1, 6, -1);
        alphaShortLabel = new QLabel(verticalLayoutWidget);
        alphaShortLabel->setObjectName(QStringLiteral("alphaShortLabel"));
        alphaShortLabel->setFont(font);

        horizontalLayout_2->addWidget(alphaShortLabel);

        alphaShortSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        alphaShortSpinBox->setObjectName(QStringLiteral("alphaShortSpinBox"));
        alphaShortSpinBox->setFont(font1);
        alphaShortSpinBox->setMaximum(1);
        alphaShortSpinBox->setSingleStep(0.01);
        alphaShortSpinBox->setValue(0.43);

        horizontalLayout_2->addWidget(alphaShortSpinBox);

        alphaLongLabel = new QLabel(verticalLayoutWidget);
        alphaLongLabel->setObjectName(QStringLiteral("alphaLongLabel"));
        alphaLongLabel->setFont(font);

        horizontalLayout_2->addWidget(alphaLongLabel);

        alphaLongSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        alphaLongSpinBox->setObjectName(QStringLiteral("alphaLongSpinBox"));
        alphaLongSpinBox->setFont(font1);
        alphaLongSpinBox->setMaximum(1);
        alphaLongSpinBox->setSingleStep(0.01);
        alphaLongSpinBox->setValue(0.25);

        horizontalLayout_2->addWidget(alphaLongSpinBox);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(6, -1, 6, -1);
        splitTetrasCheckBox = new QCheckBox(verticalLayoutWidget);
        splitTetrasCheckBox->setObjectName(QStringLiteral("splitTetrasCheckBox"));
        splitTetrasCheckBox->setFont(font);
        splitTetrasCheckBox->setChecked(true);

        horizontalLayout_3->addWidget(splitTetrasCheckBox);

        snapToPointsCheckBox = new QCheckBox(verticalLayoutWidget);
        snapToPointsCheckBox->setObjectName(QStringLiteral("snapToPointsCheckBox"));
        snapToPointsCheckBox->setFont(font);
        snapToPointsCheckBox->setChecked(true);

        horizontalLayout_3->addWidget(snapToPointsCheckBox);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(6, 0, 6, 0);
        generateTetrasButton = new QPushButton(verticalLayoutWidget);
        generateTetrasButton->setObjectName(QStringLiteral("generateTetrasButton"));
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
        TetraStuffingDialog->setWindowTitle(QApplication::translate("TetraStuffingDialog", "TetraStuffing Options", Q_NULLPTR));
        tetraSizeLabel->setText(QApplication::translate("TetraStuffingDialog", "TetraSize", Q_NULLPTR));
        alphaShortLabel->setText(QApplication::translate("TetraStuffingDialog", "AlphaShort", Q_NULLPTR));
        alphaLongLabel->setText(QApplication::translate("TetraStuffingDialog", "AlphaLong", Q_NULLPTR));
        splitTetrasCheckBox->setText(QApplication::translate("TetraStuffingDialog", "Split Tetras", Q_NULLPTR));
        snapToPointsCheckBox->setText(QApplication::translate("TetraStuffingDialog", "Snap To Points", Q_NULLPTR));
        generateTetrasButton->setText(QApplication::translate("TetraStuffingDialog", "Generate Tetrahedra", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TetraStuffingDialog: public Ui_TetraStuffingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TETRASTUFFINGDIALOG_H
