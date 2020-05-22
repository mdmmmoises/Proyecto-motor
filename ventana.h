#ifndef VENTANA_H
#define VENTANA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Ventana; }
QT_END_NAMESPACE

class Ventana : public QMainWindow
{
    Q_OBJECT

    public:
        Ventana(QWidget *parent = nullptr);
        ~Ventana();

    private slots:
        void on_Actualizar_clicked();
        void on_checkVelo_stateChanged(int arg1);
        void on_checkFrecuen_stateChanged(int arg1);
        void on_checkVoltaje_stateChanged(int arg1);
        void on_checkTemp_stateChanged(int arg1);


private:
        Ui::Ventana *ui;
};
#endif // VENTANA_H
