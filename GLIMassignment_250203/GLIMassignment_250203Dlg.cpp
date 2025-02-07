// GLIMassignment_250203Dlg.cpp
#include "pch.h"
#include "GLIMassignment_250203.h"
#include "GLIMassignment_250203Dlg.h"
#include "afxdialogex.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <atlconv.h>

// GDI+ 헤더
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 랜덤 이동 스레드 프로시저를 선언
UINT RandomMoveThreadProc(LPVOID pParam);

// 대화상자 생성자
CGLIMassignment_250203Dlg::CGLIMassignment_250203Dlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_GLIMASSIGNMENT_250203_DIALOG, pParent)
    , m_bDragging(false)                  // 드래그 상태 초기화
    , m_dragIndex(-1)                     // 드래그 인덱스 초기화
    , m_clickCircleRadius(20)             // 기본 반지름을 20으로 설정
    , m_borderThickness(3)                // 기본 두께를 3으로 설정
{
    // GDI+를 초기화
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    // 랜덤 시드를 초기화
    std::srand((unsigned)std::time(nullptr));
}

// 대화상자 소멸자
CGLIMassignment_250203Dlg::~CGLIMassignment_250203Dlg()
{
    // GDI+를 종료
    GdiplusShutdown(m_gdiplusToken);
}

// 데이터 교환을 수행
void CGLIMassignment_250203Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // 필요 시 DDX_Control 추가
}

// 메시지 맵을 정의
BEGIN_MESSAGE_MAP(CGLIMassignment_250203Dlg, CDialogEx)
    ON_WM_PAINT()                                  // OnPaint 메시지 처리
    ON_WM_QUERYDRAGICON()                          // 드래그 아이콘 요청 처리
    ON_WM_LBUTTONDOWN()                            // 왼쪽 버튼 다운 처리
    ON_WM_LBUTTONUP()                              // 왼쪽 버튼 업 처리
    ON_WM_MOUSEMOVE()                              // 마우스 이동 처리
    ON_BN_CLICKED(IDC_Reset, &CGLIMassignment_250203Dlg::OnBnClickedResetButton)    // 초기화 버튼 클릭 처리
    ON_BN_CLICKED(IDC_Random, &CGLIMassignment_250203Dlg::OnBnClickedRandomButton)  // 랜덤 이동 버튼 클릭 처리
END_MESSAGE_MAP()

// 대화상자 초기화를 수행
BOOL CGLIMassignment_250203Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 아이콘을 설정
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // 다이얼로그 타이틀을 "그림 과제"로 고정
    SetWindowText(_T("그림 과제"));

    // 클라이언트 영역 크기
    GetClientRect(&m_clientRect);

    // IDC_Radius와 IDC_Thickness에 기본값을 설정
    CString strVal;
    strVal.Format(_T("%d"), m_clickCircleRadius);
    SetDlgItemText(IDC_Radius, strVal);
    strVal.Format(_T("%d"), m_borderThickness);
    SetDlgItemText(IDC_Thickness, strVal);

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환
}

// 그리기 작업을 수행
void CGLIMassignment_250203Dlg::OnPaint()
{
    CPaintDC dc(this);  // 페인트 DC를 생성
    Graphics graphics(dc.GetSafeHdc());  // GDI+ Graphics 객체를 생성

    // IDC_Radius와 IDC_Thickness의 최신 값 읽기
    CString strVal;
    GetDlgItemText(IDC_Radius, strVal);
    m_clickCircleRadius = _ttoi(strVal);
    GetDlgItemText(IDC_Thickness, strVal);
    m_borderThickness = _ttoi(strVal);

    // 안티앨리어싱 모드를 설정
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    {
        CSingleLock lock(&m_cs, TRUE);  // 임계 구역 관련
        // 1. 클릭 지점에 원을 그림
        for (size_t i = 0; i < m_points.size(); i++)
        {
            int radius = m_points[i].radius;  // 각 원의 반지름을 사용
            int x = m_points[i].pt.x - radius;
            int y = m_points[i].pt.y - radius;
            // 검은색으로 원 내부 채우기
            Gdiplus::SolidBrush fillBrush(Color(255, 0, 0, 0));
            graphics.FillEllipse(&fillBrush, x, y, radius * 2, radius * 2);

            // 원의 중심 좌표를 문자열로 구성
            CString coord;
            coord.Format(_T("(%d, %d)"), m_points[i].pt.x, m_points[i].pt.y);
            Gdiplus::FontFamily fontFamily(L"Arial");
            Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
            Gdiplus::SolidBrush textBrush(Color(255, 0, 0, 0));
#ifdef UNICODE
            std::wstring ws(coord);
#else
            std::wstring ws(CT2W(coord));
#endif
            // 좌표 문자열을 원 오른쪽 아래에 표시
            PointF pt((Gdiplus::REAL)m_points[i].pt.x + radius, (Gdiplus::REAL)m_points[i].pt.y + radius);
            graphics.DrawString(ws.c_str(), -1, &font, pt, &textBrush);
        }

        // 2. 만약 원이 3개 이상이면 원 그리기
        if (m_points.size() >= 3)
        { //원을 그리기 위한 공식을 외접원 계산으로 한다면?
            // 3개의 점으로 외심과 반지름을 계산 0206
            //세 점 A(ax, ay), B(bx, by), C(cx, cy)
            double ax = m_points[0].pt.x, ay = m_points[0].pt.y;
            double bx = m_points[1].pt.x, by = m_points[1].pt.y;
            double cx = m_points[2].pt.x, cy = m_points[2].pt.y;
            double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
            if (fabs(d) > 1e-6)
            {
                double ux = ((ax * ax + ay * ay) * (by - cy) + //원의 x 좌표
                    (bx * bx + by * by) * (cy - ay) +
                    (cx * cx + cy * cy) * (ay - by)) / d;

                double uy = ((ax * ax + ay * ay) * (cx - bx) + //원의 y 좌표
                    (bx * bx + by * by) * (ax - cx) +
                    (cx * cx + cy * cy) * (bx - ax)) / d;

                double radius = std::sqrt((ux - ax) * (ux - ax) + (uy - ay) * (uy - ay)); //원의 반지름
                int left = (int)(ux - radius);
                int top = (int)(uy - radius);
                int diameter = (int)(radius * 2);

                // 원의 두께
                Gdiplus::Pen gardenPen(Color(255, 0, 128, 0), (Gdiplus::REAL)m_borderThickness);
                graphics.DrawEllipse(&gardenPen, left, top, diameter, diameter);
            }
        }
    }
}

// 드래그 아이콘 요청에 대해 핸들을 반환
HCURSOR CGLIMassignment_250203Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// 마우스 왼쪽 버튼을 누를 때 처리
void CGLIMassignment_250203Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    CSingleLock lock(&m_cs, TRUE);  // 임계 구역 관련
    bool bHit = false;
    // 기존 원 내부 클릭 여부를 검사
    for (size_t i = 0; i < m_points.size(); i++)
    {
        int dx = point.x - m_points[i].pt.x;
        int dy = point.y - m_points[i].pt.y;
        if (std::sqrt(dx * dx + dy * dy) <= m_points[i].radius)
        {
            // 원 내부 클릭이면 드래그 모드를 활성화
            m_bDragging = true;
            m_dragIndex = (int)i;
            bHit = true;
            break;
        }
    }
    // 원 내부가 아니고, 원의 개수가 3개 미만이면 새 원을 추가
    if (!bHit && m_points.size() < 3)
    {
        // IDC_Radius의 값을 읽어와 현재 반지름을 결정
        int curRadius = m_clickCircleRadius;
        CString strVal;
        GetDlgItemText(IDC_Radius, strVal);
        curRadius = _ttoi(strVal);

        // 새 원을 생성
        ClickCircle newCircle;
        newCircle.pt = point;
        newCircle.radius = curRadius;
        m_points.push_back(newCircle);
        // 좌표 정보를 업데이트
        UpdatePointCoordinatesDisplay();
        Invalidate();  // 화면을 갱신
    }
    // 기본 메시지 처리를 수행
    CDialogEx::OnLButtonDown(nFlags, point);
}

// 마우스 이동 시 처리
void CGLIMassignment_250203Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDragging && m_dragIndex >= 0 && m_dragIndex < (int)m_points.size())
    {
        {
            CSingleLock lock(&m_cs, TRUE);  // 임계 구역 관련
            // 드래그 중에도 원의 반지름은 그대로 유지
            m_points[m_dragIndex].pt = point;
            UpdatePointCoordinatesDisplay();  // 좌표 정보를 업데이트
        }
        Invalidate();  // 화면을 갱신
    }
    CDialogEx::OnMouseMove(nFlags, point);
}

// 마우스 왼쪽 버튼을 뗄 때 처리
void CGLIMassignment_250203Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        m_bDragging = false;   // 드래그 모드를 해제
        m_dragIndex = -1;      // 드래그 인덱스를 초기화
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

// 클릭 좌표 정보를 IDC_Value에 표시
// 다이얼로그 타이틀 대신 Static Control(IDC_Value)에 좌표 정보를 출력
void CGLIMassignment_250203Dlg::UpdatePointCoordinatesDisplay()
{
    CString strCoordinates;
    for (size_t i = 0; i < m_points.size(); i++)
    {
        CString temp;
        temp.Format(_T(" ( %d, %d, r=%d) "), m_points[i].pt.x, m_points[i].pt.y, m_points[i].radius);
        strCoordinates += temp;
    }
    // Static Text(IDC_Value)에 좌표 문자열을 출력
    SetDlgItemText(IDC_Value, strCoordinates);
}

// [초기화] 버튼 클릭 시 처리
// 모든 원을 삭제하고 좌표 정보를 갱신
void CGLIMassignment_250203Dlg::OnBnClickedResetButton()
{
    {
        CSingleLock lock(&m_cs, TRUE);  // 임계 구역 관련
        m_points.clear();               // 원 정보를 모두 삭제
    }
    UpdatePointCoordinatesDisplay();    // 좌표 정보를 업데이트
    Invalidate();                       // 화면을 갱신
}

// 랜덤 이동 스레드 프로시저 (초당 2회, 총 10회 반복)
UINT RandomMoveThreadProc(LPVOID pParam)
{
    CGLIMassignment_250203Dlg* pDlg = (CGLIMassignment_250203Dlg*)pParam;
    for (int i = 0; i < 10; i++)
    {
        {
            CSingleLock lock(&pDlg->m_cs, TRUE);  // 임계 구역 관련
            // 각 원을 임의의 위치로 이동
            for (size_t j = 0; j < pDlg->m_points.size(); j++)
            {
                int x = rand() % pDlg->m_clientRect.Width();
                int y = rand() % pDlg->m_clientRect.Height();
                pDlg->m_points[j].pt = CPoint(x, y);
            }
            pDlg->UpdatePointCoordinatesDisplay();  // 좌표 정보를 업데이트
        }
        pDlg->Invalidate();  // 화면을 갱신
        Sleep(500);         // 500ms 대기 (초당 2회)
    }
    return 0;
}

// [랜덤 이동] 버튼 클릭 시 처리
// 3개 이상의 원이 있을 때 별도 스레드에서 랜덤 이동을 수행
void CGLIMassignment_250203Dlg::OnBnClickedRandomButton()
{
    if (m_points.size() < 3)
        return;  // 원이 3개 미만이면 동작하지 않음
    AfxBeginThread(RandomMoveThreadProc, this);  // 별도 스레드에서 랜덤 이동을 시작
}
