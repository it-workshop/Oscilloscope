/****************************************************************************
** Meta object code from reading C++ file 'osc.h'
**
** Created: Thu Aug 4 19:01:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../osc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'osc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Osc[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Osc[] = {
    "Osc\0\0on_pushButton_clicked()\0"
};

const QMetaObject Osc::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Osc,
      qt_meta_data_Osc, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Osc::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Osc::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Osc::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Osc))
        return static_cast<void*>(const_cast< Osc*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Osc::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_pushButton_clicked(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
