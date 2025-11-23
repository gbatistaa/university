import { CommonEntity } from 'src/modules/common/entities/common.entity';
import { Conductor } from 'src/modules/ms-conductor/entities/conductor.entity';
import { Vehicle } from 'src/modules/ms-vehicle/entities/vehicle.entity';
import { Column, Entity, JoinColumn, ManyToOne } from 'typeorm';

@Entity()
export class Penalty extends CommonEntity {
  @Column({ type: 'text', nullable: false })
  description: string;

  @Column({ type: 'float', nullable: false })
  pontuation: number;

  @Column({ type: 'text', nullable: false })
  vehicleSign: string;

  @Column({ type: 'text', nullable: false })
  conductorCpf: string;

  @ManyToOne(() => Conductor, (conductor) => conductor.penalties)
  @JoinColumn({ name: 'conductorCpf', referencedColumnName: 'cpf' })
  conductor: Conductor;

  @ManyToOne(() => Vehicle, (vehicle) => vehicle.penalties)
  @JoinColumn({ name: 'vehicleSign', referencedColumnName: 'sign' })
  vehicle: Vehicle;
}
