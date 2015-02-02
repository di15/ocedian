
#include "entity.h"
#include "physics.h"
#include "quake3bsp.h"
#include "3dmath.h"
#include "main.h"
#include "player.h"
#include "gui.h"

void Physics()
{
	CEntity* e;
	CEntityType* t;
	CCamera* c;
	CPlayer* p;
	CVector3 old;
	CVector3 trace;

	for(int i=0; i<ENTITIES; i++)
	{
		e = &g_entity[i];
		
		if(!e->on)
			continue;

		t = &g_entityType[e->type];
		c = &e->camera;

		if(e->controller >= 0)
		{
			p = &g_player[e->controller];

			if(p->forward)
				c->Move(t->speed);
			if(p->backward)
				c->Move(-t->speed);
			if(p->left)
				c->Strafe(-t->speed);
			if(p->right)
				c->Strafe(t->speed);
			if(p->jump)
				c->Rise(t->boost);
		}
		
		c->LimitHVel(t->speed);
		old = c->Position();
		c->Step();

		trace = g_map.TraceBox(old, c->Position(), t->vMin, t->vMax, t->maxStep);

		c->MoveTo(trace);

		c->Grounded( g_map.IsOnGround() );

		if(c->Grounded())
			c->Friction();
		else
			c->Rise(-GRAVITY);

		if(g_map.Stuck() && !c->Grounded())
		{
			CVector3 vel = c->Velocity();
			vel.x = 0;
			vel.z = 0;
			c->Velocity(vel);
		}

		if(g_map.Ceiling())
		{
			CVector3 vel = c->Velocity();
			vel.y = 0;
			c->Velocity(vel);
		}
	}
}