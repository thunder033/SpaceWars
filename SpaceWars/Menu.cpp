#include "Menu.h"



Menu::Menu() : Scene("menu")
{
}


Menu::~Menu()
{
}

void Menu::init()
{
	Scene::init();
}

void Menu::update(float dt, float tt)
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		//Scene::setActive("space");
		Scene::setActive("gameScene");
	}
}

void Menu::draw(float deltaTime, float totalTime, Renderer* renderer) {
	SpriteBatch* spriteBatch = renderer->getSpriteBatch();
	SpriteFont* spriteFont = renderer->getSpriteFont();

	spriteBatch->Begin();
	std::wstring rot(L"Main Menu");
	const wchar_t* text = rot.c_str();
	spriteFont->DrawString(spriteBatch, L"SPACE WARS", XMFLOAT2(100, 100));
	spriteFont->DrawString(spriteBatch, L"Press Space to Play", XMFLOAT2(100, 130));
	spriteBatch->End();
}