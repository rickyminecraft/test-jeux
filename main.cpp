#include <cstdlib>
#include <vector>
#include <random>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "images.h"

struct Animpixels
{
	bool alive()
	{
		live--;
		pos.x += direction.x;
		pos.y += direction.y;
		if (live == 0)
		{
			return false;
		}
		return true;
	}
	void setspeed()
	{
		// Seed with a real random value, if available
		std::random_device r;
		std::random_device r2;

		std::default_random_engine engine(r());
		std::default_random_engine engine2(r2());
		std::uniform_real_distribution<float> uniform_dist(1.0f, 1.5f);
		std::uniform_int_distribution<int> uniform_dist1(5, 150);
		speed = uniform_dist(engine);
		live = uniform_dist1(engine2);
	}
	void setdirection()
	{
		std::random_device r;
		std::default_random_engine engine(r());
		std::uniform_real_distribution<float> uniform_dist(0.0f, 359.0f);
		float rotation = uniform_dist(engine);
		float angle = rotation * 3.14f / 180.f;
		direction.x = cos(angle) * speed;
		direction.y = sin(angle) * speed;
	}
	void setpos(sf::Vector2i positioni)
	{
		pos.x = (float)positioni.x;
		pos.y = (float)positioni.y;
	}
	void setcolor(sf::Color couleur)
	{
		couleurp = couleur;
	}
	sf::Vector2f getpos()
	{
		return pos;
	}
	sf::Color getcolor()
	{
		return couleurp;
	}
private:
	sf::Color couleurp;
	sf::Vector2f pos;
	sf::Vector2f direction;
	short live = 0;
	float speed = 0;
};

struct Bombe
{
	bool isalive()
	{
		pos.y += speed;
		return alive;
	}
	void setspeed()
	{
		// Seed with a real random value, if available
		std::random_device r;

		std::default_random_engine engine(r());
		std::uniform_real_distribution<float> uniform_dist(1.0f, 1.5f);
		speed = uniform_dist(engine);
	}
	void setpos(sf::Vector2i positioni)
	{
		pos.x = (float)positioni.x;
		pos.y = (float)positioni.y;
	}

	sf::Vector2f getpos()
	{
		return pos;
	}

	void hashit(sf::Image image)
	{

		//x, y = haut gauche
		for (int x = (int)pos.x; x < pos.x + 20; x++)
		{
			for (float step = pos.y; step < (pos.y + 10 + speed); step++)
			{
				if (step < 600)
				{
					sf::Color couleur = image.getPixel(x, (unsigned int)step);
					sf::Color couleur2 = sf::Color(255, 255, 255, 0);
					if (couleur != sf::Color::Transparent)
					{
						alive = false;
					}
				}
				else
				{
					alive = false;
				}
			}
		}
	}

private:
	bool alive = true;
	bool contact = false;
	sf::Vector2f pos;
	float speed = 0;
};

int main()
{
	sf::VideoMode Mode = sf::VideoMode(800, 600);
	sf::RenderWindow *MainWindows = new sf::RenderWindow(Mode, "test", sf::Style::Close);
	//necessaire pour avoir un framerate constant
	MainWindows->setVerticalSyncEnabled(true);

	short game_mode = 1;

	if (game_mode == 1)
	{
		sf::Image fond, fond2, cercle1;
		sf::Texture texture, t2, cercleT;
		sf::Sprite sprite, sp2, cercleS;

		//fond.loadFromFile("fond1.png");
		fond.loadFromMemory(&cfond1, sizeof(cfond1));
		//fond2.loadFromFile("fond.png");
		fond2.loadFromMemory(&cfond, sizeof(cfond));
		//cercle1.loadFromFile("cercle2.png");
		cercle1.loadFromMemory(&ccercle2, sizeof(ccercle2));

		texture.loadFromImage(fond);
		t2.loadFromImage(fond2);
		cercleT.loadFromImage(cercle1);

		sprite.setTexture(texture);
		sp2.setTexture(t2);
		cercleS.setTexture(cercleT);

		bool Alive = true;
		bool click = false;

		std::vector<Animpixels> pixelsa;

		sf::Event event;

		while (Alive)
		{
			MainWindows->clear();

			MainWindows->pollEvent(event);
			if (event.type == sf::Event::Closed)
			{
				Alive = false;
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				click = true;
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				click = false;
			}
			if (event.type == sf::Event::MouseMoved && click)
			{
				sf::Vector2i position;
				position.x = event.mouseMove.x - 30;//taile image diviser par 2, pour que ça soit centrer par rapport a la souris
				position.y = event.mouseMove.y - 30;
				sf::Vector2u pixelcount = cercle1.getSize();
				for (int x = pixelcount.x; x > 0; x--)
				{
					for (int y = pixelcount.y; y > 0; y--)
					{
						sf::Color positioncolor = cercle1.getPixel(x - 1, y - 1);
						if (positioncolor != sf::Color::Black)
						{
							if (position.x + x < (int)fond.getSize().x && position.x + x >= 0 && position.y + y < (int)fond.getSize().y && position.y + y >= 0)
							{
								if (fond.getPixel(position.x + x, position.y + y) != sf::Color::Transparent)
								{
									Animpixels apix;
									apix.setcolor(fond.getPixel(position.x + x, position.y + y));
									apix.setpos(sf::Vector2i(position.x + x, position.y + y));
									apix.setspeed();
									apix.setdirection();
									pixelsa.push_back(apix);
									fond.setPixel(position.x + x, position.y + y, sf::Color::Transparent);//l'idée est de faire un trou
								}
							}
						}
					}
				}
				//après avoir modifier l'image, on actualise la texture
				texture.loadFromImage(fond);
			}

			MainWindows->draw(sp2);
			MainWindows->draw(sprite);
			
			if (!pixelsa.empty())
			{
				//std::printf("%zu \r\n", pixelsa.size());
				sf::VertexArray vertexarray(sf::Points);
				sf::Vertex vertex;
				for (std::size_t boucle = pixelsa.size(); boucle > 0; boucle--)
				{
					if (pixelsa[boucle - 1].alive())
					{
						vertex.color = pixelsa[boucle - 1].getcolor();
						vertex.position = pixelsa[boucle - 1].getpos();
						vertexarray.append(vertex);
					}
					else
					{
						pixelsa.erase(pixelsa.cbegin() + (boucle - 1));
					}
				}
				MainWindows->draw(vertexarray);
				//vertexarray.resize(0);
			}

			MainWindows->display();
		}
	}
	else if (game_mode == 2)
	{
		sf::Image fond, fond2, bombe, cercle, cercle2;
		sf::Texture t1, t2, b1;
		sf::Sprite sp1, sp2, sp3;
		//fond.loadFromFile("fond_bleu.png");
		fond.loadFromMemory(&cfondbleu, sizeof(cfondbleu));
		//fond2.loadFromFile("fond2.png");
		fond2.loadFromMemory(&cfond2, sizeof(cfond2));
		//bombe.loadFromFile("bombe.png");
		bombe.loadFromMemory(&bombes, sizeof(bombes));
		//cercle.loadFromFile("cercle1.png");
		cercle.loadFromMemory(&ccercle1, sizeof(ccercle1));
		//cercle2.loadFromFile("cercle2.png");
		cercle2.loadFromMemory(&ccercle2, sizeof(ccercle2));

		t1.loadFromImage(fond);
		t2.loadFromImage(fond2);
		b1.loadFromImage(bombe);

		sp1.setTexture(t1);
		sp2.setTexture(t2);
		sp3.setTexture(b1);

		bool Alive = true;
		bool click = false;
		short count = 0;
		short bigcercle = 0;

		std::vector<Animpixels> pixelsa;
		std::vector<Bombe> bombes;

		sf::Event event;

		while (Alive)
		{
			MainWindows->clear();

			MainWindows->pollEvent(event);
			switch (event.type)
			{
			case sf::Event::Closed:
				Alive = false;
				break;
			case sf::Event::MouseButtonPressed:
				//contre mesure pour clic continue  
				if (count == 0)
				{
					click = true;
				}
				count++;
				break;
			case sf::Event::MouseButtonReleased:
				count = 0;
				break;
			}

			//fait apparaitre une bombe
			if (click)
			{
				sf::Vector2i position;
				position.x = event.mouseButton.x - 10;//taille image diviser par 2, pour que ça soit centrer par rapport a la souris
				position.y = event.mouseButton.y - 10;
				Bombe thebombe;
				thebombe.setspeed();
				thebombe.setpos(sf::Vector2i(position.x, position.y));
				bombes.push_back(thebombe);
				click = false;
			}

			//dessine les fonds
			MainWindows->draw(sp1);
			MainWindows->draw(sp2);

			//si au moins une bombe existe
			if (bombes.size())
			{
				for (std::size_t boucle = bombes.size(); boucle > 0; boucle--)
				{
					bombes[boucle - 1].hashit(fond2);
					if (bombes[boucle - 1].isalive())
					{
						sp3.setPosition(bombes[boucle - 1].getpos());
						MainWindows->draw(sp3);
					}
					else
					{
						bigcercle++;
						sf::Vector2i position;
						sf::Vector2u pixelcount;
						if (bigcercle == 4)
						{
							position.x = (int)bombes[boucle - 1].getpos().x - 20;//taile image diviser par 2, pour que ça soit centrer par rapport a la souris
							position.y = (int)bombes[boucle - 1].getpos().y - 20;
							pixelcount = cercle2.getSize();
						}
						else
						{
							position.x = (int)bombes[boucle - 1].getpos().x - 10;//taile image diviser par 2, pour que ça soit centrer par rapport a la souris
							position.y = (int)bombes[boucle - 1].getpos().y - 10;
							pixelcount = cercle.getSize();
						}
						for (int x = pixelcount.x; x > 0; x--)
						{
							for (int y = pixelcount.y; y > 0; y--)
							{
								sf::Color positioncolor;
								if (bigcercle == 4)
								{
									positioncolor = cercle2.getPixel(x - 1, y - 1);
								}
								else
								{
									positioncolor = cercle.getPixel(x - 1, y - 1);
								}
								if (positioncolor != sf::Color::Black)
								{
									if (position.x + x < (int)fond2.getSize().x && position.x + x >= 0 && position.y + y < (int)fond2.getSize().y && position.y + y >= 0)
									{
										if (fond2.getPixel(position.x + x, position.y + y) != sf::Color::Transparent)
										{
											Animpixels apix;
											apix.setcolor(fond2.getPixel(position.x + x, position.y + y));
											apix.setpos(sf::Vector2i(position.x + x, position.y + y));
											apix.setspeed();
											apix.setdirection();
											pixelsa.push_back(apix);
											fond2.setPixel(position.x + x, position.y + y, sf::Color::Transparent);//l'idée est de faire un trou
										}
									}
								}
							}
						}
						if (bigcercle >= 4)
						{
							bigcercle = 0;
						}
						//après avoir modifier l'image, on actualise la texture
						
						bombes.erase(bombes.cbegin() + (boucle - 1));
					}
				}
				t2.loadFromImage(fond2);
			}

			//si on a des particules
			if (!pixelsa.empty())
			{

				sf::VertexArray vertexarray(sf::Points);
				for (std::size_t boucle = pixelsa.size(); boucle > 0; boucle--)
				{
					if (pixelsa[boucle - 1].alive())
					{
						sf::Vertex vertex;
						vertex.color = pixelsa[boucle - 1].getcolor();
						vertex.position = pixelsa[boucle - 1].getpos();
						vertexarray.append(vertex);
					}
					else
					{
						pixelsa.erase(pixelsa.cbegin() + (boucle - 1));
					}
				}
				MainWindows->draw(vertexarray);
				vertexarray.clear();
			}
			MainWindows->display();
		}
	}
	else
	{
		sf::Image fond, front;
		sf::Texture t1, t2;
		sf::Sprite sp1, sp2;
		sf::RenderTexture texture;

		sf::Font fonte;
		sf::Text texte;
		sf::String thestring;

		fonte.loadFromFile("arial.ttf");
		texte.setFont(fonte);
		texte.setFillColor(sf::Color::White);
		texte.setOutlineColor(sf::Color::White);
		texte.setCharacterSize(40);
		texte.setStyle(sf::Text::Regular);
		texte.setPosition(10, 300);

		fond.create(800, 600, sf::Color::Black);
		front.create(800, 600, sf::Color::Black);
		t1.loadFromImage(fond);
		t2.loadFromImage(fond);

		sp1.setTexture(t1);
		sp2.setTexture(t2);

		char alph[] = { 'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,'H' ,'I' ,'J' ,'K' ,'L' ,'M' ,'N' ,'O' ,'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'W' ,'X' ,'Y' ,'Z' };

		sf::Event event;

		bool Alive = true;
		bool doeffect = false;
		bool destroy = false;

		sf::Vector2i position{ 5, 295 };
		std::vector<Animpixels> pixelsa;

		int xpos = 0;

		while (Alive)
		{
			MainWindows->clear();

			MainWindows->pollEvent(event);
			switch (event.type)
			{
			case sf::Event::Closed:
				Alive = false;
				break;
				//si on appuie sur une touche
			case sf::Event::KeyPressed:
				//si c'est une lettre
				if (event.key.code < 26)
				{
					short charpos = event.key.code;
					thestring += alph[charpos];
					texte.setString(thestring);
				}
				//si c'est la touche entré
				if (event.key.code == sf::Keyboard::Enter)
				{
					doeffect = true;
				}
				//si c'est la barre espace
				if (event.key.code == sf::Keyboard::Space)
				{
					thestring += " ";
				}
				//si c'est retour arrière
				if (event.key.code == sf::Keyboard::BackSpace)
				{
					std::size_t taille = thestring.getSize();
					thestring = thestring.substring(0,taille -1);
					texte.setString(thestring);
				}
				//si c'est la touche 0 du clavier numérique
				//on interrompte la destruction
				if (event.key.code == sf::Keyboard::Numpad0)
				{
					if (doeffect && destroy)
					{
						doeffect = false;
						destroy = false;
						xpos = 0;
					}
				}
				
				break;
			}

			//on dessine le fond noir
			MainWindows->draw(sp1);

			//si on a appuyer sur entré, on lance l'effet de particule
			//sinon on ecrit juste le texte
			if (!doeffect)
			{
				MainWindows->draw(texte);
			}
			else
			{
				//si c'est la premiere fois, on initialise une texture de rendu
				//ou on va copier le texte. texture.display sert a rendre en interne la texture
				//pour que l'on puisse après la mettre dans l'image front
				//puis on efface la texture et on met le texte a rien ""
				if (!destroy)
				{
					texture.create(800, 600);
					texture.draw(texte);
					texture.display();
					t2 = texture.getTexture();
					front = t2.copyToImage();
					texture.clear();
					destroy = true;
					thestring = "";
					texte.setString(thestring);
				}
			}

			//si on detruit le texte
			if (destroy)
			{
				//on boucle de bas en haut, ligne par ligne
				for (int i = 0; i < 50; i++)
				{
					//si le pixel n'est pas noir, on ajoute une particule
					if (front.getPixel(position.x + xpos, position.y + i) != sf::Color::Black)
					{
						Animpixels apix;
						apix.setcolor(front.getPixel(position.x + xpos, position.y + i));
						apix.setpos(sf::Vector2i(position.x + xpos, position.y + i));
						apix.setspeed();
						apix.setdirection();
						pixelsa.push_back(apix);
						//et on efface le pixel en question
						front.setPixel(position.x + xpos, position.y + i, sf::Color::Black);//l'idée est de faire un trou
					}
				}
				//on actualise la texture t2 et on affiche le sprite lier a la texture
				//puis on avance d'une colonne (xpo++)
				t2.loadFromImage(front);
				MainWindows->draw(sp2);
				xpos++;
			}
			
			//si on a des particules, on ls affiches et on les anime aussi
			if (!pixelsa.empty())
			{

				sf::VertexArray vertexarray(sf::Points);
				for (std::size_t boucle = pixelsa.size(); boucle > 0; boucle--)
				{
					if (pixelsa[boucle - 1].alive())
					{
						sf::Vertex vertex;
						vertex.color = pixelsa[boucle - 1].getcolor();
						vertex.position = pixelsa[boucle - 1].getpos();
						vertexarray.append(vertex);
					}
					else
					{
						pixelsa.erase(pixelsa.cbegin() + (boucle - 1));
					}
				}
				MainWindows->draw(vertexarray);
				vertexarray.clear();
			}

			//si on detruit
			if (destroy)
			{
				//et si on est arrivé au bout
				if (xpos >= 799)
				{
					//on remet tout a zéro
					destroy = false;
					doeffect = false;
					xpos = 0;
				}
			}

			MainWindows->display();
		}
	}

	return EXIT_SUCCESS;
}