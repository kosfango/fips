// bmpview.cpp : implementation file
// IDD_BMP_VIEW
#include "stdafx.h"
#include "dibapi.h"
#include "lightdlg.h"
#include "bmpview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char		*MemoryPlace=0;
CPalette*	FarbpPalette=0;
CSize		m_sizeDoc;
void		free_dib_stuff();
int			find_mm_description(LPSTR mm_element,CString &desc);

bmpview		*pBmpView=0;
extern CLightDlg *gpMain;
extern _gconfig gc;

// ========================================================================
	bmpview::bmpview(CWnd* pParent ) : CDialog(bmpview::IDD, pParent)
// ========================================================================
{
	//{{AFX_DATA_INIT(bmpview)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ========================================================================
	void bmpview::DoDataExchange(CDataExchange* pDX)
// ========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(bmpview)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(bmpview, CDialog)
	//{{AFX_MSG_MAP(bmpview)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ========================================================================
	BOOL bmpview::OnInitDialog()
// ========================================================================
{
	CDialog::OnInitDialog();
	pBmpView=this;
	return TRUE;
}

// ========================================================================
	void bmpview::OnPaint()
// ========================================================================
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

// ========================================================================
	void bmpview::OnRButtonDown(UINT nFlags, CPoint point)
// ========================================================================
{
	ShowWindow(SW_HIDE);
	gpMain->m_subjlist.SetFocus();
	CDialog::OnRButtonDown(nFlags, point);
}

// ========================================================================
	int	display_bitmap(const char *path)
// ========================================================================
{
static int done=0;
CString    desc;
RECT	re,client,newpos;
int		bmp_width,bmp_heigth,frame_size,x=0,y=0;
double	k;
const char	*p;

	if (!SetBitmapToDisplay(path))
		return 0;

	if (!done)
	{
		done=1;
		gpMain->GetClientRect(&re);
		gpMain->ClientToScreen(&re);
		x=get_cfg(CFG_BITMAP,"Xpos",re.right);
		y=get_cfg(CFG_BITMAP,"Ypos",re.top);
		if (x<re.left || x>re.right)	x=re.right;
		if (y<re.top || y>re.bottom)	y=re.top;
		gc.save_posnow=1;
	}
	else
	{
		pBmpView->GetWindowRect(&re);
		x=re.right;
		y=re.top;
	}

	bmp_width  = (int) ::DIBWidth(MemoryPlace);
	bmp_heigth = (int) ::DIBHeight(MemoryPlace);

	// calculate sizes of frames
	pBmpView->GetWindowRect(&re);
	pBmpView->GetClientRect(&client);
	pBmpView->ClientToScreen(&client);
	frame_size=client.top-re.top;

	newpos.top=y;
	newpos.right=x;

	switch (gc.bmps_size)
	{
	case 0:
	default:
		k=1;
		break;
	case 1:
		k=1.5;
		break;
	case 2:
		k=2;
		break;
	case 3:
		k=2.8;
	}
	newpos.left=x-int(bmp_width/k);
	newpos.bottom=y+frame_size+int(bmp_heigth/k);
	pBmpView->MoveWindow(&newpos);
	pBmpView->ShowWindow(SW_SHOWNORMAL);

	p=strrchr(path,'\\');
	if (p)
	{
		p++;
		find_mm_description((LPSTR)p,desc);
		desc.OemToAnsi();
		pBmpView->SetWindowText(desc);
	}
	force_bmp_display_update();
	gpMain->m_subjlist.SetFocus();
	return 1;
}

// ========================================================================
	void hide_mm_bitmap(void)
// ========================================================================
{
	pBmpView->ShowWindow(SW_HIDE);
	free_dib_stuff();
	gpMain->m_subjlist.SetFocus();
}

// ======================================================
	BOOL SetBitmapToDisplay(LPCTSTR lpszPathName)
// ======================================================
{
CFile			file;
CFileException	fe;

	free_dib_stuff();

	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
		return 0;

	TRY
	{
		if (!::ReadDIBFile(file))
		{
			free_dib_stuff();
			return 0;
		}
	}
	CATCH (CFileException, eLoad)
	{
		file.Abort(); // will not throw an exception
		free_dib_stuff();
		return 0;
	}
	END_CATCH

	if (!InitDIBData())
	{
		free_dib_stuff();
		return 0;
	}
	return 1;
}

// ======================================================
	int ReadDIBFile(CFile &file)
// ======================================================
{
BITMAPFILEHEADER bmfHeader;
DWORD dwBitsSize;

	dwBitsSize = (DWORD)file.GetLength();
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))	return 0;
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))	return 0;
	ALLOC0(MemoryPlace,dwBitsSize);
	if (file.Read(MemoryPlace, dwBitsSize - sizeof(BITMAPFILEHEADER)) != 
		dwBitsSize - sizeof(BITMAPFILEHEADER) )	return 0;

	return 1;
}

// ======================================================
	int InitDIBData()
// ======================================================
{

	if (FarbpPalette != NULL)
	{
		delete FarbpPalette;
		FarbpPalette = NULL;
	}
	if (!MemoryPlace)
		return 0;

	LPSTR lpDIB = (LPSTR) MemoryPlace;
	if (DIBWidth(lpDIB) > INT_MAX || DIBHeight(lpDIB) > INT_MAX)
		return 0;

	m_sizeDoc = CSize((int) DIBWidth(lpDIB), (int) DIBHeight(lpDIB));

	FarbpPalette = new CPalette;
	if (!FarbpPalette)
		return 0;

	if (!CreateDIBPalette(MemoryPlace, FarbpPalette))
		return 0;

	return 1;
}


// ======================================================
	void free_dib_stuff()
// ======================================================
{
	if (FarbpPalette)
	{
		delete FarbpPalette;
		FarbpPalette = 0;
	}

	if (MemoryPlace)
	{
		free (MemoryPlace);
		MemoryPlace=0;
	}
}


// ======================================================
	void bmpview::OnDraw(CDC* pDC)
// ======================================================
{
	if (MemoryPlace)
	{
		LPSTR lpDIB = (LPSTR) MemoryPlace;
		int cxDIB = (int) ::DIBWidth(lpDIB);         // Size of DIB - x
		int cyDIB = (int) ::DIBHeight(lpDIB);        // Size of DIB - y
		CRect rcDIB;
		rcDIB.top = rcDIB.left = 0;					 // Links oben!
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;
		CRect rcDest;
		rcDest = rcDIB;
		::PaintDIB(pDC->m_hDC, &rcDest,lpDIB,&rcDIB,FarbpPalette);
	}
}

// ======================================================
	void bmpview::OnMove(int x, int y)
// ======================================================
{
RECT	re;
RECT	client;
double	size;
int		frame_size;
int		cxDIB;

	CDialog::OnMove(x, y);

	if (!gc.save_posnow)
		return;

	// get right corner
	cxDIB = (int) ::DIBWidth(MemoryPlace);

	switch (gc.bmps_size)
	{
		case 0:
		default:
			size=cxDIB;
			break;
		case 1:
			size=cxDIB/1.5;
			break;
		case 2:
			size=cxDIB/2;
			break;
		case 3:
			size=cxDIB/2.8;
			break;
	}

	GetWindowRect(&re);
	GetClientRect(&client);
	ClientToScreen(&client);
	frame_size=client.top-re.top;

	set_cfg(CFG_BITMAP,"Xpos",x+(int)size-2);
	set_cfg(CFG_BITMAP,"Ypos",y-frame_size);
}

// =========================================================
	void force_bmp_display_update(void )
// =========================================================
{
RECT re;

	pBmpView->GetClientRect(&re);
	pBmpView->InvalidateRect(&re);
}

//===================================================
// search description for Multimedia Element
	int find_mm_description(char *mm_element,CString &desc)
//===================================================
{
int		len,ret,num=0;
char	name[100],buf[300],mmdir[MAX_PATH];
FILE	*fp;

	desc.Empty();
	sscanf(mm_element,"%x",&num);
	num=num/256;
	sprintf(buf,"FMMP%04x.inf",num);
	make_path(mmdir,gc.MultimedPath,buf);
	fp=fopen(mmdir,"rt");
	if (fp)
	{
		len=strlen(mm_element);
		while (fgets(buf,299,fp))
		{
			if (!strnicmp(buf,mm_element,len))
			{
				ret=sscanf(buf,"%*[^,]%*[, ]%[^,\n]",name);
				if (ret==1)
				{
					desc=name;
					fclose(fp);
					return 1;
				}
				break;
			}
		}
		fclose(fp);
	}
	return 0;
}
