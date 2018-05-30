/********************************************************************************
** Form generated from reading UI file 'quartettetrastuffingdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUARTETTETRASTUFFINGDIALOG_H
#define UI_QUARTETTETRASTUFFINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
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

class Ui_QuartetTetraStuffingDialog
{
public:
    QDialogButtonBox *buttonBox;
    QFrame *frame;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *gridSpacingLabel;
    QDoubleSpinBox *gridSpacingSpinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *angleThresholdtLabel;
    QDoubleSpinBox *angleThresholdSpinBox;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *generateTetrasButton;

    void setupUi(QDialog *QuartetTetraStuffingDialog)
    {
        if (QuartetTetraStuffingDialog->objectName().isEmpty())
            QuartetTetraStuffingDialog->setObjectName(QStringLiteral("QuartetTetraStuffingDialog"));
        QuartetTetraStuffingDialog->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QuartetTetraStuffingDialog->sizePolicy().hasHeightForWidth());
        QuartetTetraStuffingDialog->setSizePolicy(sizePolicy);
        QuartetTetraStuffingDialog->setMinimumSize(QSize(400, 300));
        QuartetTetraStuffingDialog->setMaximumSize(QSize(400, 300));
        buttonBox = new QDialogButtonBox(QuartetTetraStuffingDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        frame = new QFrame(QuartetTetraStuffingDialog);
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
        gridSpacingLabel = new QLabel(verticalLayoutWidget);
        gridSpacingLabel->setObjectName(QStringLiteral("gridSpacingLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        gridSpacingLabel->setFont(font);

        horizontalLayout->addWidget(gridSpacingLabel);

        gridSpacingSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        gridSpacingSpinBox->setObjectName(QStringLiteral("gridSpacingSpinBox"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        gridSpacingSpinBox->setFont(font1);
        gridSpacingSpinBox->setSingleStep(0.01);
        gridSpacingSpinBox->setValue(0.3);

        horizontalLayout->addWidget(gridSpacingSpinBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(6, -1, 6, -1);
        angleThresholdtLabel = new QLabel(verticalLayoutWidget);
        angleThresholdtLabel->setObjectName(QStringLiteral("angleThresholdtLabel"));
        angleThresholdtLabel->setFont(font);

        horizontalLayout_2->addWidget(angleThresholdtLabel);

        angleThresholdSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        angleThresholdSpinBox->setObjectName(QStringLiteral("angleThresholdSpinBox"));
        angleThresholdSpinBox->setFont(font1);
        angleThresholdSpinBox->setMaximum(90);
        angleThresholdSpinBox->setSingleStep(0.1);
        angleThresholdSpinBox->setValue(15);

        horizontalLayout_2->addWidget(angleThresholdSpinBox);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(6, 0, 6, 0);
        generateTetrasButton = new QPushButton(verticalLayoutWidget);
        generateTetrasButton->setObjectName(QStringLiteral("generateTetrasButton"));
        generateTetrasButton->setFont(font);

        horizontalLayout_4->addWidget(generateTetrasButton);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(QuartetTetraStuffingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), QuartetTetraStuffingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), QuartetTetraStuffingDialog, SLOT(reject()));
        QObject::connect(generateTetrasButton, SIGNAL(released()), QuartetTetraStuffingDialog, SLOT(generateTetras()));

        QMetaObject::connectSlotsByName(QuartetTetraStuffingDialog);
    } // setupUi

    void retranslateUi(QDialog *QuartetTetraStuffingDialog)
    {
        QuartetTetraStuffingDialog->setWindowTitle(QApplication::translate("QuartetTetraStuffingDialog", "Quartet TetraStuffing Options", Q_NULLPTR));
        gridSpacingLabel->setText(QApplication::translate("QuartetTetraStuffingDialog", "Cell Size", Q_NULLPTR));
        angleThresholdtLabel->setText(QApplication::translate("QuartetTetraStuffingDialog", "Angle Threshold", Q_NULLPTR));
        generateTetrasButton->setText(QApplication::translate("QuartetTetraStuffingDialog", "Generate Tetrahedra", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QuartetTetraStuffingDialog: public Ui_QuartetTetraStuffingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUARTETTETRASTUFFINGDIALOG_H
