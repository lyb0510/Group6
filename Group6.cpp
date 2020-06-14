#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES	1
#define ANIMATION_TIME		0.01f
#define ANIMATION_STEP		2       //팩맨 고스트 움직임 속도
#define INF 100000

#include <bangtal.h>
#include <stdio.h>
#include <stdlib.h>

//object크기 : 유령, 팩맨 : 25*25, 코인 : 10*10, 아이템 : 20*20 

struct obj {
	ObjectID object;
	int x;
	int y;
	// 화면상 위치
	int mapx;
	int mapy;
	// 맵상에서 위치(0 <= mapx,mapy <= 18)
	char vec;
	// 움직임 방향 (N : 정지, R : 우, L : 좌, U : 위, D : 아래)
};

int dx = 0, dy = 0, count = 0; // x,y 델타값
char dvec = 'N'; //입력된 벡터값

int dx_ghost[4], dy_ghost[4]; // 유령의 델타값
int score;

int wall_x[20] = { 92, 104, 145, 189, 214, 255, 280, 320, 345, 387, 412, 453, 478, 519, 545, 584, 610, 653, 693, 707 };
int wall_y[20] = { 685, 643, 599, 555, 534, 490, 465, 425, 400, 360, 334, 294, 268, 228, 202, 161, 136, 95, 71, 30 };
//벽 좌표


int wall[21][19] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
	{1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1},
	{1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
	{1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1},
	{1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1},
	{1,1,1,1,0,1,0,1,1,2,1,1,0,1,0,1,1,1,1},
	{0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0},
	{1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
	{1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1},
	{1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
	{1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
	{1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
	{1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1},
	{1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
	{1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};// 0 : 길, 1 : 벽, 2 : 일방통행
//팩맨 초기 좌표 : [15][9]
//유령 초기 좌표 : [8][9] , [9][8], [9][9], [9][10]
// map[8][9]에 일방통행 해야하는 구간 존재[i][j]에서 i>=7인 곳에 있으면 통과 가능(빈공간), 그렇지 않다면 벽과 동일

int map_item[21][19] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0},
	{0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,1,0},
	{0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0},
	{0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{4,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,4},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,1,0},
	{0,2,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,2,0},
	{0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0},
	{0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0},
	{0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
int map_item_org[21][19] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0},
	{0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,1,0},
	{0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0},
	{0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{4,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,4},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
	{0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0},
	{0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,1,0},
	{0,2,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,2,0},
	{0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0},
	{0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0},
	{0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}; //복사용
//팩맨 초기 좌표 : [15][9]
// (i,j)위치 코인 -> (Wall_x[i]+Wall_x[i+1])/2 - 코인x크기/2, (Wall_y[j - 1]+Wall_y[j])/2 - 코인y크기/2 에 생성 코인 8*8아이템 12*12
// 코인, 아이템, (팩맨이 가장 최근에 위치한 장소...?) ,통로  좌표정보 표시
// 없음 : 0 , 코인 : 1, 아이템 : 2, 팩맨 최근 위치 : 3, 통로 좌표 : 4


SceneID scene1, scene2;
ObjectID startButton, endButton, coin[21][19]; //coin[]은 오브젝트 이미지만 활용할거라 실질적으로 저기에 coin,item 둘다 들어가요.
obj pacman, ghost[4];

TimerID Timer;

int medval(int a, int b, int objsize) {
	return (a + b - objsize) / 2;
} //obj중앙에 배치용 함수에요

void setplayerVEC() {
	int u = 0, l = 0, d = 0, r = 0;
	if (wall[pacman.mapy - 1][pacman.mapx] == 0) u = 1;
	if (wall[pacman.mapy][pacman.mapx - 1] == 0) l = 1;
	if (wall[pacman.mapy + 1][pacman.mapx] == 0) d = 1;
	if (wall[pacman.mapy][pacman.mapx + 1] == 0) r = 1;

	if (dvec == 'U' && u) {
		pacman.vec = 'U';
	}
	if (dvec == 'L' && l) {
		pacman.vec = 'L';
	}
	if (dvec == 'D' && d) {
		pacman.vec = 'D';
	}
	if (dvec == 'R' && r) {
		pacman.vec = 'R';
	}
}// 여기도 dvec에 입력값이 잘 안먹는거 같은데...요...
//코드랑 애니메이션의 차이 때문에 발생하는 걸까요? ㅠㅠ

void setghostVEC(int n) {
	int u = 0, l = 0, d = 0, r = 0;
	int ud = INF, ld = INF, dd = INF, rd = INF, min = INF;
	char vec = ghost[n].vec;
	if (wall[ghost[n].mapy - 1][ghost[n].mapx] == 0) {
		u = 1;
	}
	if (wall[ghost[n].mapy][ghost[n].mapx - 1] == 0) {
		l = 1;
	}
	if (wall[ghost[n].mapy + 1][ghost[n].mapx] == 0) {
		d = 1;
	}
	if (wall[ghost[n].mapy][ghost[n].mapx + 1] == 0) {
		r = 1;
	}
	if (u + r + d + l > 1) {//(u & r) || (r & d) || (d & l) || (l & u)) {
		if (u) {
			ud = (ghost[n].mapy - 1) + ghost[n].mapx - pacman.mapy - pacman.mapx;
			if (min > ud) {
				vec = 'U', min = ud;
			}
		}
		if (l) {
			ld = ghost[n].mapy + (ghost[n].mapx - 1) - pacman.mapy - pacman.mapx;
			if (min > ld) {
				vec = 'L', min = ld;
			}
		}
		if (d) {
			dd = (ghost[n].mapy + 1) + ghost[n].mapx - pacman.mapy - pacman.mapx;
			if (min > dd) {
				vec = 'D', min = dd;
			}
		}
		if (r) {
			rd = ghost[n].mapy + (ghost[n].mapx + 1) - pacman.mapy - pacman.mapx;
			if (min > rd) {
				vec = 'R', min = rd;
			}
		}
	}
	ghost[n].vec = vec;
	//바꾼 방향으로 이동 
}
// 갈림길 or 벽에서 유령ghost[n].vec 설정
// 분기별 (가로+세로)거리 체크해서 이동
// 길이가 같으면 (u -> l -> d -> r 순서로 이동)

void game_init() {
	// coin
	int idx_coin = 0;
	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 19; j++) {
			if (map_item[i][j] == 1) {
				// 가로로 j번째, 세로로 i 번째; 
				coin[i][j] = createObject("Images\\coin.png");
				scaleObject(coin[i][j], 0.5f);
				locateObject(coin[i][j], scene2, medval(wall_x[j], wall_x[j + 1], 10), medval(wall_y[i - 1], wall_y[i], 10));
				showObject(coin[i][j]);
			}
			else if (map_item[i][j] == 2) {
				// 가로로 j번째, 세로로 i 번째; 
				coin[i][j] = createObject("Images\\item.png");
				locateObject(coin[i][j], scene2, medval(wall_x[j], wall_x[j + 1], 20), medval(wall_y[i - 1], wall_y[i], 20));
				showObject(coin[i][j]);
			}
		}
	}

	// ghost
	for (int i = 0; i < 4; i++) {
		char filename[30] = "";
		sprintf_s(filename, "Images\\ghost\\ghost%d.png", i);
		ghost[i].object = createObject(filename);
		if (i == 0) {
			ghost[i].x = medval(wall_x[9], wall_x[10], 25);
			ghost[i].y = medval(wall_y[7], wall_y[8], 25);
			ghost[i].mapx = 9;
			ghost[i].mapy = 8;
		}
		else {
			ghost[i].x = medval(wall_x[7 + i], wall_x[8 + i], 25);
			ghost[i].y = medval(wall_y[8], wall_y[9], 25);
			ghost[i].mapx = 7 + i;
			ghost[i].mapy = 9;
		}
		ghost[i].vec = 'U';
		locateObject(ghost[i].object, scene2, ghost[i].x, ghost[i].y);
		showObject(ghost[i].object);
	}

	//player (= pacman)
	pacman.object = createObject("Images\\player\\playerR.png");
	pacman.x = medval(wall_x[9], wall_x[10], 25);
	pacman.y = medval(wall_y[14], wall_y[15], 25);
	pacman.mapx = 9;
	pacman.mapy = 15;
	pacman.vec = 'N';
	locateObject(pacman.object, scene2, pacman.x, pacman.y);
	showObject(pacman.object);

}
// 각 물체의 초기 상태 설정

void g_moving() {
	switch (ghost[0].vec) {
	case 'N':
		setghostVEC(0);
		dx_ghost[0] = 0; dy_ghost[0] = 0;
		break;
	case 'U':
		if (wall[ghost[0].mapy - 1][ghost[0].mapx] == 1) {
			dx_ghost[0] = 0; dy_ghost[0] = 0;
			ghost[0].vec = 'N';
			setghostVEC(0);
			ghost[0].y = medval(wall_y[ghost[0].mapy - 1], wall_y[ghost[0].mapy], 25);
			return;
		}
		else {
			dy_ghost[0] = ANIMATION_STEP; dx = 0;
		}
		break;
	case 'D':
		if (wall[ghost[0].mapy + 1][ghost[0].mapx] == 1) {
			dx_ghost[0] = 0; dy_ghost[0] = 0;
			ghost[0].vec = 'N';
			setghostVEC(0);
			ghost[0].y = medval(wall_y[ghost[0].mapy - 1], wall_y[ghost[0].mapy], 25);
			return;
		}
		else {
			dy_ghost[0] = -ANIMATION_STEP; dx = 0;
		}
		break;
	case 'R':
		if (wall[ghost[0].mapy][ghost[0].mapx + 1] == 1 && ghost[0].x <= medval(wall_x[ghost[0].mapx], wall_x[ghost[0].mapx - 1], 25)) {
			dx_ghost[0] = 0; dy_ghost[0] = 0;
			ghost[0].vec = 'N';
			setghostVEC(0);
			ghost[0].x = medval(wall_x[ghost[0].mapx], wall_x[ghost[0].mapx - 1], 25);
			return;
		}
		else {
			dx_ghost[0] = ANIMATION_STEP; dy = 0;
		}
		break;
	case 'L':
		if (wall[ghost[0].mapy][ghost[0].mapx - 1] == 1 && ghost[0].x >= wall_x[ghost[0].mapx]) {
			dx_ghost[0] = 0; dy_ghost[0] = 0;
			ghost[0].vec = 'N';
			setghostVEC(0);
			ghost[0].x = medval(wall_x[ghost[0].mapx], wall_x[ghost[0].mapx + 1], 25);
			return;
		}
		else {
			dx_ghost[0] = -ANIMATION_STEP; dy = 0;
		}
		break;
	}
}// 일단 이걸로 1마리만 움직여 봤는데 잘 안되서 빼는게 좋을거 같아요 ㅠㅠ
// 동작 에러가 더 심해져요...ㅠㅠ

void p_moving() {
	switch (pacman.vec) {
	case 'N':
		dx = 0; dy = 0;
		break;
	case 'U':
		setObjectImage(pacman.object, "Images\\player\\playerU.png");
		if (wall[pacman.mapy - 1][pacman.mapx] == 1) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.y = medval(wall_y[pacman.mapy - 1], wall_y[pacman.mapy], 25);
			return;
		}
		else {
			dy = ANIMATION_STEP; dx = 0;
		}
		break;
	case 'D':
		setObjectImage(pacman.object, "Images\\player\\playerD.png");
		if (wall[pacman.mapy + 1][pacman.mapx] == 1) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.y = medval(wall_y[pacman.mapy - 1], wall_y[pacman.mapy], 25);
			return;
		}
		else {
			dy = -ANIMATION_STEP; dx = 0;
		}
		break;
	case 'R':
		setObjectImage(pacman.object, "Images\\player\\playerR.png");
		if (wall[pacman.mapy][pacman.mapx + 1] == 1 && pacman.x <= medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25)) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.x = medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25);
			return;
		}
		else {
			dx = ANIMATION_STEP; dy = 0;
		}
		break;
	case 'L':
		setObjectImage(pacman.object, "Images\\player\\playerL.png");
		if (wall[pacman.mapy][pacman.mapx - 1] == 1 && pacman.x >= wall_x[pacman.mapx]) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.x = medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25);
			return;
		}
		else {
			dx = -ANIMATION_STEP; dy = 0;
		}
		break;
	}
}

void setNewGame() {
	score = 0;
	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 19; j++) {
			map_item[i][j] = map_item_org[i][j];
		}
	}
	hideObject(pacman.object);
	for (int i = 0; i < 4; i++) {
		hideObject(ghost[i].object);
	}
}

void getScore() {
	if (map_item[pacman.mapy][pacman.mapx] == 1) {
		hideObject(coin[pacman.mapy][pacman.mapx]);
		score += 100;
		map_item[pacman.mapy][pacman.mapx] = 0;
	}
	else if (map_item[pacman.mapy][pacman.mapx] == 2) {
		hideObject(coin[pacman.mapy][pacman.mapx]);
		score += 200;
		map_item[pacman.mapy][pacman.mapx] = 0;
	}
	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 19; j++) {
			if (map_item[i][j] == 0) count++;
		}
	}
	if (count == 397) {
		char message[25] = "";
		sprintf_s(message, "Your Score : [%d]", score);
		showMessage(message);
		setNewGame();
		enterScene(scene1);
	}
	else count = 0;
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	if (object == startButton) {
		enterScene(scene2);
		game_init();
	}
	else if (object == endButton) {
		endGame();
	}
}

void keyboardCallback(KeyCode code, KeyState state) {
	switch (code) {
	case 84://위
		dvec = 'U';
		setplayerVEC();
		p_moving();
		break;
	case 85://아래
		dvec = 'D';
		setplayerVEC();
		p_moving();
		break;
	case 83://오른쪽
		dvec = 'R';
		setplayerVEC();
		p_moving();
		break;
	case 82://왼쪽
		dvec = 'L';
		setplayerVEC();
		p_moving();
		break;
	}
	// x,y 좌표 비교로 벽만남 추가 + 이동하면서 pacman.mapx, pacman.mapy변화 기록 ...?
	// 개인적으로는 분기나 벽까지 해당 백터값(vec)로 계속 이동하다가 분기나 벽 만나면 벡터의 변화를 판단하게 하는게 유령에도 응용하기 좋을 것 같아요

}//키보드 움직임

void timerCallback(TimerID timer)
{
	if (pacman.vec == 'R' && map_item[pacman.mapy][pacman.mapx] == 4) {
		pacman.mapx = 1;
		pacman.x = medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25);
	}
	if (pacman.vec == 'L' && map_item[pacman.mapy][pacman.mapx] == 4) {
		pacman.mapx = 18;
		pacman.x = medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25);
	}
	if (pacman.vec == 'U' && medval(wall_y[pacman.mapy - 1], wall_y[pacman.mapy], 25) < pacman.y) {
		if (wall[pacman.mapy - 1][pacman.mapx] == 1) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.y = medval(wall_y[pacman.mapy - 1], wall_y[pacman.mapy], 25);
			pacman.mapy++;
		}
		pacman.mapy--;
	}
	else if (pacman.vec == 'D' && wall_y[pacman.mapy] > pacman.y) {
		if (wall[pacman.mapy + 1][pacman.mapx] == 1) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.y = medval(wall_y[pacman.mapy - 1], wall_y[pacman.mapy], 25);
			pacman.mapy--;
		}
		pacman.mapy++;
	}
	else if (pacman.vec == 'R' && medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25) < pacman.x) {
		if (wall[pacman.mapy][pacman.mapx + 1] == 1) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.x = medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25);
			pacman.mapx--;
		}
		pacman.mapx++;
	}
	else if (pacman.vec == 'L' && wall_x[pacman.mapx] > pacman.x) {
		if (wall[pacman.mapy][pacman.mapx - 1] == 1) {
			dx = 0; dy = 0;
			pacman.vec = 'N';
			pacman.x = medval(wall_x[pacman.mapx], wall_x[pacman.mapx + 1], 25);
			pacman.mapx++;
		}
		pacman.mapx--;
	}

	pacman.x += dx; pacman.y += dy;
	locateObject(pacman.object, scene2, pacman.x, pacman.y);
	getScore();
	//printf("%d %d\n", pacman.mapx, pacman.mapy);


	setTimer(timer, ANIMATION_TIME);
	startTimer(timer);
}// 타이머 콜백 


int main()
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);
	setKeyboardCallback(keyboardCallback);

	scene1 = createScene("Title", "Images\\Title.png");
	scene2 = createScene("PAC-MAN", "Images\\map.png");

	startButton = createObject("Images\\start.png");
	locateObject(startButton, scene1, 390, 100);
	scaleObject(startButton, 2.0f);
	showObject(startButton);

	endButton = createObject("Images\\end.png");
	locateObject(endButton, scene1, 690, 100);
	scaleObject(endButton, 2.0f);
	showObject(endButton);

	TimerID timer = createTimer(ANIMATION_TIME);
	startTimer(timer);//팩맨 & 귀신 움직임 타이머

	startGame(scene1);
}