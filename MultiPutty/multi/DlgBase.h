
#pragma once

class MyDialog {
  HWND _dlg;

public:
  int RunModal(HINSTANCE resModule, UINT resId, HWND parent){
    return DialogBoxParam(resModule, MAKEINTRESOURCE(resId), parent, &StaticDialogProc, (LPARAM)this);
  }

protected:
  static INT_PTR CALLBACK StaticDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
    MyDialog* self;
    if (uMsg == WM_INITDIALOG){
      self = (MyDialog*)lParam;
      self->_dlg = hwndDlg;
      SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);
    }
    else
      self = (MyDialog*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);

    if (self)
      return self->DialogProc(uMsg, wParam, lParam);

    return FALSE;
  }

  virtual UINT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
    case WM_INITDIALOG:
      OnInitDialog();
      break;
    case WM_COMMAND:
      OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;
    default:
      return FALSE;
    }
    return TRUE;
  }

  virtual void OnInitDialog(){
  }

  virtual void OnCommand(int id, USHORT notifyCode, HWND control){
    EndDialog(_hdlg, id);
  }

};