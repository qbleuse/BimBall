// Fill out your copyright notice in the Description page of Project Settings.


#include "KillzoneDummy.h"
#include "BimBall/Entity/Entity.h"

void AKillzoneDummy::OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector normalImpulse, const FHitResult& hit)
{
	if (AEntity* entity = Cast<AEntity>(otherActor))
	{
		if (IsUp())
		{
			/* Staggers*/
			FVector projImpulse = hit.Normal;
			projImpulse.Z = 0;
			hitStaggerData.knockbackDir = (hit.Normal + projImpulse).GetSafeNormal();
			entity->Stagger(hitStaggerData);

			/* Deal Damages */
			entity->DealDamages(this, damagesOnHit);
		}
	}
}