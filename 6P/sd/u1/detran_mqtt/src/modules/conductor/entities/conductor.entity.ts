import { CommonEntity } from 'src/modules/common/entities/common.entity';
import { Vehicle } from 'src/modules/vehicle/entities/vehicle.entity';
import { Column, Entity, OneToMany, Unique } from 'typeorm';

@Entity()
@Unique(['cpf'])
export class Conductor extends CommonEntity {
  @Column({ type: 'text', nullable: false })
  cpf: string;

  @Column({ type: 'text', nullable: false })
  name: string;

  @OneToMany(() => Vehicle, (vehicle) => vehicle.conductor)
  veiculos: Vehicle[];
}
