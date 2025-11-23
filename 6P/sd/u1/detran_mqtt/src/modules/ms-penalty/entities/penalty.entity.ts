import { CommonEntity } from 'src/modules/common/entities/common.entity';
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

  @ManyToOne(() => Vehicle, (vehicle) => vehicle.penalties)
  @JoinColumn({ name: 'vehicleSign', referencedColumnName: 'sign' })
  vehicle: Vehicle;
}
