// GLIMassignment_250203Dlg.h : header file
//

#pragma once
#include <vector>
#include <gdiplus.h>
using namespace Gdiplus; //0203

// 클릭 원 정보를 저장할 구조체 선언 0204
struct ClickCircle {
    CPoint pt;   // 원의 중심 좌표
    int radius;  // 해당 원의 반지름
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
    // 마우스 이벤트
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

    // 버튼 핸들러
    afx_msg void OnBnClickedResetButton(); // [초기화] 버튼 (IDC_Reset)
    afx_msg void OnBnClickedRandomButton(); // [랜덤 이동] 버튼 (IDC_Random)

public:
    // 클릭 지점 (최대 3개)
    std::vector<ClickCircle> m_points; //0204
    // 드래그 상태 관련
    bool m_bDragging;
    int  m_dragIndex;

    // Edit Control에서 입력받은 값들
    int m_clickCircleRadius; // IDC_Radius에서 입력 (클릭 지점 원의 반지름)
    int m_borderThickness;   // IDC_Thickness에서 입력 (정원의 외곽 두께)

    // GDI+ 관련 토큰
    ULONG_PTR m_gdiplusToken;

    // 클라이언트 영역 크기 (랜덤 이동 범위)
    CRect m_clientRect;

    // 동기화를 위한 임계 구역 (스레드 안전)
    CCriticalSection m_cs;

    // UI에 클릭 지점 좌표를 표시하는 헬퍼 함수 (예제에서는 타이틀 바 업데이트)
    void UpdatePointCoordinatesDisplay();
};