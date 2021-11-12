#include <QtNetwork>
#include <QtWidgets>
#include <QApplication>
#include <iostream>

int main(int argc, char **argv)
{
    QApplication app{ argc, argv };

    QUdpSocket  connection;
    QWidget     root;
    QVBoxLayout layout{ &root };
    QFormLayout layoutForm;

    bool isCapturing = false;
    QString captureTextDefault = QStringLiteral("Capture");
    QString capturingText = QStringLiteral("Stop");

    QPushButton capture_button{ captureTextDefault };
    QLineEdit   interfaces;
    QLineEdit   groupNet;
    QSpinBox port;
    QHostAddress currentHost;
    QFile fileOutput;

    port.setMaximum(std::numeric_limits<uint16_t>::max());

    layout.addLayout(&layoutForm);
    layout.addWidget(&capture_button);

    layoutForm.addRow("Multicast Group", &groupNet);
    layoutForm.addRow("Multicast Port", &port);

    QObject::connect(&capture_button,  &QPushButton::clicked, [&](bool){
        if(isCapturing) {
            connection.leaveMulticastGroup(currentHost);
            connection.close();

            fileOutput.write("]}");
            fileOutput.close();

            capture_button.setText(captureTextDefault);

            interfaces.setEnabled(true);
            groupNet.setEnabled(true);
            port.setEnabled(true);
        }
        else {
            auto save = QFileDialog::getSaveFileName(&root, QStringLiteral("save file"), {}, QStringLiteral(".txt"));

            if(save.isEmpty())
                return;

            interfaces.setEnabled(false);
            groupNet.setEnabled(false);
            port.setEnabled(false);

            fileOutput.setFileName(save);
            fileOutput.open(QFile::WriteOnly | QFile::Truncate);
            fileOutput.write(R"({"otherData":{},"traceEvents":[{})");

            currentHost.setAddress(groupNet.text());
            capture_button.setText(capturingText);
            connection.bind(QHostAddress::AnyIPv4, port.value(), QUdpSocket::ShareAddress);
            connection.joinMulticastGroup(currentHost);
        }
        isCapturing = !isCapturing;
    });

    QObject::connect(&root, &QWidget::destroyed, [&](QObject*){
        if(connection.isOpen()) {
            connection.leaveMulticastGroup(currentHost);
            connection.close();

            fileOutput.write("]}");
            fileOutput.close();
        }
    });

    QObject::connect(&connection, &QUdpSocket::readyRead, [&] {
        fileOutput.write(connection.receiveDatagram().data());
    });

    root.show();

    return app.exec();
}