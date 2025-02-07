// GLIMassignment_250203Dlg.h : header file
//

#pragma once
#include <vector>
#include <gdiplus.h>
using namespace Gdiplus; //0203

// Ŭ�� �� ������ ������ ����ü ���� 0204
struct ClickCircle {
    CPoint pt;   // ���� �߽� ��ǥ
    int radius;  // �ش� ���� ������
};

// CGLIMassignment_250203Dlg dialog
class CGLIMassignment_250203Dlg : public CDialogEx
{
    // Construction
public:
    CGLIMassignment_250203Dlg(CWnd* pParent = nullptr);    // standard constructor
    virtual ~CGLIMassignment_250203Dlg(); //0203

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_GLIMASSIGNMENT_250203_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public: //0203
    // ���콺 �̺�Ʈ
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

    // ��ư �ڵ鷯
    afx_msg void OnBnClickedResetButton(); // [�ʱ�ȭ] ��ư (IDC_Reset)
    afx_msg void OnBnClickedRandomButton(); // [���� �̵�] ��ư (IDC_Random)

public:
    // Ŭ�� ���� (�ִ� 3��)
    std::vector<ClickCircle> m_points; //0204
    // �巡�� ���� ����
    bool m_bDragging;
    int  m_dragIndex;

    // Edit Control���� �Է¹��� ����
    int m_clickCircleRadius; // IDC_Radius���� �Է� (Ŭ�� ���� ���� ������)
    int m_borderThickness;   // IDC_Thickness���� �Է� (������ �ܰ� �β�)

    // GDI+ ���� ��ū
    ULONG_PTR m_gdiplusToken;

    // Ŭ���̾�Ʈ ���� ũ�� (���� �̵� ����)
    CRect m_clientRect;

    // ����ȭ�� ���� �Ӱ� ���� (������ ����)
    CCriticalSection m_cs;

    // UI�� Ŭ�� ���� ��ǥ�� ǥ���ϴ� ���� �Լ� (���������� Ÿ��Ʋ �� ������Ʈ)
    void UpdatePointCoordinatesDisplay();
};