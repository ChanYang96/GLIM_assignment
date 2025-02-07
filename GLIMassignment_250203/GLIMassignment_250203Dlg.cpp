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

// GDI+ ���
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� �̵� ������ ���ν����� ����
UINT RandomMoveThreadProc(LPVOID pParam);

// ��ȭ���� ������
CGLIMassignment_250203Dlg::CGLIMassignment_250203Dlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_GLIMASSIGNMENT_250203_DIALOG, pParent)
    , m_bDragging(false)                  // �巡�� ���� �ʱ�ȭ
    , m_dragIndex(-1)                     // �巡�� �ε��� �ʱ�ȭ
    , m_clickCircleRadius(20)             // �⺻ �������� 20���� ����
    , m_borderThickness(3)                // �⺻ �β��� 3���� ����
{
    // GDI+�� �ʱ�ȭ
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    // ���� �õ带 �ʱ�ȭ
    std::srand((unsigned)std::time(nullptr));
}

// ��ȭ���� �Ҹ���
CGLIMassignment_250203Dlg::~CGLIMassignment_250203Dlg()
{
    // GDI+�� ����
    GdiplusShutdown(m_gdiplusToken);
}

// ������ ��ȯ�� ����
void CGLIMassignment_250203Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // �ʿ� �� DDX_Control �߰�
}

// �޽��� ���� ����
BEGIN_MESSAGE_MAP(CGLIMassignment_250203Dlg, CDialogEx)
    ON_WM_PAINT()                                  // OnPaint �޽��� ó��
    ON_WM_QUERYDRAGICON()                          // �巡�� ������ ��û ó��
    ON_WM_LBUTTONDOWN()                            // ���� ��ư �ٿ� ó��
    ON_WM_LBUTTONUP()                              // ���� ��ư �� ó��
    ON_WM_MOUSEMOVE()                              // ���콺 �̵� ó��
    ON_BN_CLICKED(IDC_Reset, &CGLIMassignment_250203Dlg::OnBnClickedResetButton)    // �ʱ�ȭ ��ư Ŭ�� ó��
    ON_BN_CLICKED(IDC_Random, &CGLIMassignment_250203Dlg::OnBnClickedRandomButton)  // ���� �̵� ��ư Ŭ�� ó��
END_MESSAGE_MAP()

// ��ȭ���� �ʱ�ȭ�� ����
BOOL CGLIMassignment_250203Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // �������� ����
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // ���̾�α� Ÿ��Ʋ�� "�׸� ����"�� ����
    SetWindowText(_T("�׸� ����"));

    // Ŭ���̾�Ʈ ���� ũ��
    GetClientRect(&m_clientRect);

    // IDC_Radius�� IDC_Thickness�� �⺻���� ����
    CString strVal;
    strVal.Format(_T("%d"), m_clickCircleRadius);
    SetDlgItemText(IDC_Radius, strVal);
    strVal.Format(_T("%d"), m_borderThickness);
    SetDlgItemText(IDC_Thickness, strVal);

    return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ
}

// �׸��� �۾��� ����
void CGLIMassignment_250203Dlg::OnPaint()
{
    CPaintDC dc(this);  // ����Ʈ DC�� ����
    Graphics graphics(dc.GetSafeHdc());  // GDI+ Graphics ��ü�� ����

    // IDC_Radius�� IDC_Thickness�� �ֽ� �� �б�
    CString strVal;
    GetDlgItemText(IDC_Radius, strVal);
    m_clickCircleRadius = _ttoi(strVal);
    GetDlgItemText(IDC_Thickness, strVal);
    m_borderThickness = _ttoi(strVal);

    // ��Ƽ�ٸ���� ��带 ����
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    {
        CSingleLock lock(&m_cs, TRUE);  // �Ӱ� ���� ����
        // 1. Ŭ�� ������ ���� �׸�
        for (size_t i = 0; i < m_points.size(); i++)
        {
            int radius = m_points[i].radius;  // �� ���� �������� ���
            int x = m_points[i].pt.x - radius;
            int y = m_points[i].pt.y - radius;
            // ���������� �� ���� ä���
            Gdiplus::SolidBrush fillBrush(Color(255, 0, 0, 0));
            graphics.FillEllipse(&fillBrush, x, y, radius * 2, radius * 2);

            // ���� �߽� ��ǥ�� ���ڿ��� ����
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
            // ��ǥ ���ڿ��� �� ������ �Ʒ��� ǥ��
            PointF pt((Gdiplus::REAL)m_points[i].pt.x + radius, (Gdiplus::REAL)m_points[i].pt.y + radius);
            graphics.DrawString(ws.c_str(), -1, &font, pt, &textBrush);
        }

        // 2. ���� ���� 3�� �̻��̸� �� �׸���
        if (m_points.size() >= 3)
        { //���� �׸��� ���� ������ ������ ������� �Ѵٸ�?
            // 3���� ������ �ܽɰ� �������� ��� 0206
            //�� �� A(ax, ay), B(bx, by), C(cx, cy)
            double ax = m_points[0].pt.x, ay = m_points[0].pt.y;
            double bx = m_points[1].pt.x, by = m_points[1].pt.y;
            double cx = m_points[2].pt.x, cy = m_points[2].pt.y;
            double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
            if (fabs(d) > 1e-6)
            {
                double ux = ((ax * ax + ay * ay) * (by - cy) + //���� x ��ǥ
                    (bx * bx + by * by) * (cy - ay) +
                    (cx * cx + cy * cy) * (ay - by)) / d;

                double uy = ((ax * ax + ay * ay) * (cx - bx) + //���� y ��ǥ
                    (bx * bx + by * by) * (ax - cx) +
                    (cx * cx + cy * cy) * (bx - ax)) / d;

                double radius = std::sqrt((ux - ax) * (ux - ax) + (uy - ay) * (uy - ay)); //���� ������
                int left = (int)(ux - radius);
                int top = (int)(uy - radius);
                int diameter = (int)(radius * 2);

                // ���� �β�
                Gdiplus::Pen gardenPen(Color(255, 0, 128, 0), (Gdiplus::REAL)m_borderThickness);
                graphics.DrawEllipse(&gardenPen, left, top, diameter, diameter);
            }
        }
    }
}

// �巡�� ������ ��û�� ���� �ڵ��� ��ȯ
HCURSOR CGLIMassignment_250203Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// ���콺 ���� ��ư�� ���� �� ó��
void CGLIMassignment_250203Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    CSingleLock lock(&m_cs, TRUE);  // �Ӱ� ���� ����
    bool bHit = false;
    // ���� �� ���� Ŭ�� ���θ� �˻�
    for (size_t i = 0; i < m_points.size(); i++)
    {
        int dx = point.x - m_points[i].pt.x;
        int dy = point.y - m_points[i].pt.y;
        if (std::sqrt(dx * dx + dy * dy) <= m_points[i].radius)
        {
            // �� ���� Ŭ���̸� �巡�� ��带 Ȱ��ȭ
            m_bDragging = true;
            m_dragIndex = (int)i;
            bHit = true;
            break;
        }
    }
    // �� ���ΰ� �ƴϰ�, ���� ������ 3�� �̸��̸� �� ���� �߰�
    if (!bHit && m_points.size() < 3)
    {
        // IDC_Radius�� ���� �о�� ���� �������� ����
        int curRadius = m_clickCircleRadius;
        CString strVal;
        GetDlgItemText(IDC_Radius, strVal);
        curRadius = _ttoi(strVal);

        // �� ���� ����
        ClickCircle newCircle;
        newCircle.pt = point;
        newCircle.radius = curRadius;
        m_points.push_back(newCircle);
        // ��ǥ ������ ������Ʈ
        UpdatePointCoordinatesDisplay();
        Invalidate();  // ȭ���� ����
    }
    // �⺻ �޽��� ó���� ����
    CDialogEx::OnLButtonDown(nFlags, point);
}

// ���콺 �̵� �� ó��
void CGLIMassignment_250203Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDragging && m_dragIndex >= 0 && m_dragIndex < (int)m_points.size())
    {
        {
            CSingleLock lock(&m_cs, TRUE);  // �Ӱ� ���� ����
            // �巡�� �߿��� ���� �������� �״�� ����
            m_points[m_dragIndex].pt = point;
            UpdatePointCoordinatesDisplay();  // ��ǥ ������ ������Ʈ
        }
        Invalidate();  // ȭ���� ����
    }
    CDialogEx::OnMouseMove(nFlags, point);
}

// ���콺 ���� ��ư�� �� �� ó��
void CGLIMassignment_250203Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        m_bDragging = false;   // �巡�� ��带 ����
        m_dragIndex = -1;      // �巡�� �ε����� �ʱ�ȭ
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

// Ŭ�� ��ǥ ������ IDC_Value�� ǥ��
// ���̾�α� Ÿ��Ʋ ��� Static Control(IDC_Value)�� ��ǥ ������ ���
void CGLIMassignment_250203Dlg::UpdatePointCoordinatesDisplay()
{
    CString strCoordinates;
    for (size_t i = 0; i < m_points.size(); i++)
    {
        CString temp;
        temp.Format(_T(" ( %d, %d, r=%d) "), m_points[i].pt.x, m_points[i].pt.y, m_points[i].radius);
        strCoordinates += temp;
    }
    // Static Text(IDC_Value)�� ��ǥ ���ڿ��� ���
    SetDlgItemText(IDC_Value, strCoordinates);
}

// [�ʱ�ȭ] ��ư Ŭ�� �� ó��
// ��� ���� �����ϰ� ��ǥ ������ ����
void CGLIMassignment_250203Dlg::OnBnClickedResetButton()
{
    {
        CSingleLock lock(&m_cs, TRUE);  // �Ӱ� ���� ����
        m_points.clear();               // �� ������ ��� ����
    }
    UpdatePointCoordinatesDisplay();    // ��ǥ ������ ������Ʈ
    Invalidate();                       // ȭ���� ����
}

// ���� �̵� ������ ���ν��� (�ʴ� 2ȸ, �� 10ȸ �ݺ�)
UINT RandomMoveThreadProc(LPVOID pParam)
{
    CGLIMassignment_250203Dlg* pDlg = (CGLIMassignment_250203Dlg*)pParam;
    for (int i = 0; i < 10; i++)
    {
        {
            CSingleLock lock(&pDlg->m_cs, TRUE);  // �Ӱ� ���� ����
            // �� ���� ������ ��ġ�� �̵�
            for (size_t j = 0; j < pDlg->m_points.size(); j++)
            {
                int x = rand() % pDlg->m_clientRect.Width();
                int y = rand() % pDlg->m_clientRect.Height();
                pDlg->m_points[j].pt = CPoint(x, y);
            }
            pDlg->UpdatePointCoordinatesDisplay();  // ��ǥ ������ ������Ʈ
        }
        pDlg->Invalidate();  // ȭ���� ����
        Sleep(500);         // 500ms ��� (�ʴ� 2ȸ)
    }
    return 0;
}

// [���� �̵�] ��ư Ŭ�� �� ó��
// 3�� �̻��� ���� ���� �� ���� �����忡�� ���� �̵��� ����
void CGLIMassignment_250203Dlg::OnBnClickedRandomButton()
{
    if (m_points.size() < 3)
        return;  // ���� 3�� �̸��̸� �������� ����
    AfxBeginThread(RandomMoveThreadProc, this);  // ���� �����忡�� ���� �̵��� ����
}
