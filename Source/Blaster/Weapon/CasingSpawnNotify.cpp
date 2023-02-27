// Fill out your copyright notice in the Description page of Project Settings.


#include "CasingSpawnNotify.h"
#include "Weapon.h"
#include "Engine/SKeletalMeshSocket.h"
#include "Casing.h"
#include "Blaster/Character/BlasterCharacter.h"

/*
void UCasingSpawnNotify::SpawnCasingNotify()
{
    if (BlasterCharacter)
    {
        AWeapon* Weapon = Cast<AWeapon>(BlasterCharacter->GetEquippedWeapon());
        if (Weapon)
        {
            const USkeletalMeshSocket* AmmoEjectSocket = Weapon->GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
            if (AmmoEjectSocket)
            {
                FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(Weapon->GetWeaponMesh());

                UWorld* World = GetWorld();
                if (World)
                {
                    World->SpawnActor<ACasing>(
                        CasingClass,
                        SocketTransform.GetLocation(),
                        SocketTransform.GetRotation().Rotator()
                        );
                }
            }
        }
    }
}
*/
